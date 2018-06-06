#pragma once

#include <fstream>
#include <chainbase/db_state.hpp>

#include <scorum/snapshot/data_struct_hash.hpp>
#include <scorum/snapshot/get_types_by_id.hpp>

//#include <fc/io/json.hpp>

namespace scorum {
namespace snapshot {

using db_state = chainbase::db_state;

template <class IterationTag, class Section> class save_index_visitor
{
public:
    using result_type = void;

    save_index_visitor(std::ofstream& fstream, db_state& state)
        : _fstream(fstream)
        , _state(state)
    {
    }

    template <class T> void operator()(const T&) const
    {
        static const int debug_id = 9;

        using object_type = typename T::type;

        std::cerr << "saving " << object_type::type_id << ": " << boost::core::demangle(typeid(object_type).name())
                  << std::endl;

        const auto& index = _state.template get_index<typename chainbase::get_index_type<object_type>::type>()
                                .indices()
                                .template get<IterationTag>();
        size_t sz = index.size();
        object_ids_type obj_ids;
        obj_ids.reserve(sz);
        for (auto itr = index.begin(); itr != index.end(); ++itr)
        {
            const object_type& obj = (*itr);

            obj_ids.insert(obj.id._id);

            std::cerr << '(' << obj.id._id << ')';
        }
        if (!obj_ids.empty())
        {
            std::cerr << std::endl;
        }

        fc::raw::pack(_fstream, obj_ids);
        auto itr = index.begin();
        if (sz > 0)
        {
            fc::raw::pack(_fstream, get_data_struct_hash(*itr));

            for (auto itr = index.begin(); itr != index.end(); ++itr)
            {
                const object_type& obj = (*itr);

                if (debug_id == object_type::type_id)
                {
                    fc::variant vo;
                    fc::to_variant(obj, vo);
                    std::cerr << "saved " << boost::core::demangle(typeid(object_type).name()) << ":"
                              << fc::json::to_pretty_string(vo) << std::endl;
                }

                fc::raw::pack(_fstream, obj);
            }
        }
    }

private:
    std::ofstream& _fstream;
    db_state& _state;
};

template <class IterationTag, class Section>
void save_index_section(std::ofstream& fstream, chainbase::db_state& state, const Section& section)
{
    fc::ripemd160::encoder check_enc;
    fc::raw::pack(check_enc, section.name);
    fc::raw::pack(fstream, check_enc.result());

    state.for_each_index_key([&](uint16_t index_id) {
        bool initialized = false;
        auto v = section.get_object_type_variant(index_id, initialized);
        // checking because static variant interpret uninitialized state like first type
        if (initialized)
            v.visit(save_index_visitor<IterationTag, Section>(fstream, state));
    });
}
}
}
