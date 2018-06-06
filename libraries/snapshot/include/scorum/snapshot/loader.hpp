#pragma once

#include <fstream>
#include <chainbase/db_state.hpp>

#include <scorum/snapshot/data_struct_hash.hpp>
#include <scorum/snapshot/get_types_by_id.hpp>

#include <fc/io/json.hpp>

namespace scorum {
namespace snapshot {

using db_state = chainbase::db_state;

template <class IterationTag, class Section> class load_index_visitor
{
public:
    using result_type = void;

    load_index_visitor(std::ifstream& fstream, db_state& state)
        : _fstream(fstream)
        , _state(state)
    {
    }

    template <class T> void operator()(const T&) const
    {
        static const int debug_id = 9;

        using object_type = typename T::type;

        std::cerr << "loading " << object_type::type_id << ": " << boost::core::demangle(typeid(object_type).name())
                  << std::endl;

        object_ids_type obj_ids;
        fc::raw::unpack(_fstream, obj_ids);

        if (!obj_ids.empty())
        {
            for (auto id : obj_ids)
            {
                std::cerr << '(' << id << ')';
            }
            std::cerr << std::endl;
        }

        if (obj_ids.size() > 0)
        {
            if (debug_id == object_type::type_id)
            {
                std::cerr << object_type::type_id << std::endl;
            }

            fc::ripemd160 check, etalon;

            fc::raw::unpack(_fstream, check);

            const object_type* petalon_obj = _state.template find<object_type>();
            if (petalon_obj == nullptr)
            {
                const object_type& fake_obj = _state.template create<object_type>(
                    [&](object_type& obj) { etalon = get_data_struct_hash(obj); });
                _state.template remove(fake_obj);
            }
            else
            {
                etalon = get_data_struct_hash(*petalon_obj);
            }

            FC_ASSERT(check == etalon);

            using object_id_type = typename object_type::id_type;
            using object_ref_type = std::reference_wrapper<const object_type>;
            using objects_type = std::vector<object_ref_type>;

            const auto& index = _state.template get_index<typename chainbase::get_index_type<object_type>::type>()
                                    .indices()
                                    .template get<IterationTag>();

            objects_type objs_to_remove;
            objs_to_remove.reserve(index.size());

            for (auto itr = index.begin(); itr != index.end(); ++itr)
            {
                const object_type& obj = (*itr);

                if (obj_ids.find(obj.id._id) == obj_ids.end())
                {
                    objs_to_remove.emplace_back(std::cref(obj));
                }
            }

            object_ids_type temp;
            temp.reserve(objs_to_remove.size());

            for (const object_type& obj : objs_to_remove)
            {
                temp.insert(obj.id._id);
                _state.template remove(obj);
            }

            if (!temp.empty())
            {
                std::cerr << "remove: ";
                for (auto id : temp)
                {
                    std::cerr << '(' << id << ')';
                }
                std::cerr << std::endl;
            }

            for (size_t ci = 0; ci < obj_ids.size(); ++ci)
            {
                object_id_type obj_id(*obj_ids.nth(ci));
                const object_type* pobj = _state.template find<object_type>(obj_id);
                if (pobj == nullptr)
                {
                    _state.template create<object_type>([&](object_type& obj) {
                        fc::raw::unpack(_fstream, obj);
                        if (debug_id == object_type::type_id)
                        {
                            fc::variant vo;
                            fc::to_variant(obj, vo);
                            std::cerr << "created " << boost::core::demangle(typeid(object_type).name()) << ": "
                                      << fc::json::to_pretty_string(vo) << std::endl;
                        }
                    });
                }
                else
                {
                    _state.template modify<object_type>(*pobj, [&](object_type& obj) {
                        fc::raw::unpack(_fstream, obj);
                        if (debug_id == object_type::type_id)
                        {
                            fc::variant vo;
                            fc::to_variant(obj, vo);
                            std::cerr << "updated " << boost::core::demangle(typeid(object_type).name()) << ":"
                                      << fc::json::to_pretty_string(vo) << std::endl;
                        }
                    });
                }
            }
        }
    }

private:
    std::ifstream& _fstream;
    db_state& _state;
};

template <class IterationTag, class Section>
void load_index_section(std::ifstream& fstream,
                        chainbase::db_state& state,
                        scorum::snapshot::index_ids_type& loaded_idxs,
                        const Section& section)
{
    fc::ripemd160 check;

    fc::raw::unpack(fstream, check);

    fc::ripemd160::encoder check_enc;
    fc::raw::pack(check_enc, section.name);

    FC_ASSERT(check_enc.result() == check);

    state.for_each_index_key([&](uint16_t index_id) {
        bool initialized = false;
        auto v = section.get_object_type_variant(index_id, initialized);
        // checking because static variant interpret uninitialized state like first type
        if (initialized)
        {
            v.visit(load_index_visitor<IterationTag, Section>(fstream, state));
            loaded_idxs.insert(index_id);
        }
    });
}
}
}
