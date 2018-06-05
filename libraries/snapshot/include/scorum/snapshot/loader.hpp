#pragma once

#include <fstream>
#include <chainbase/db_state.hpp>

#include <scorum/snapshot/data_struct_hash.hpp>
#include <scorum/snapshot/get_types_by_id.hpp>

//#include <fc/io/json.hpp>

namespace scorum {
namespace snapshot {

using db_state = chainbase::db_state;

template <class Section> class load_index_visitor
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
        using object_type = typename T::type;

        std::cerr << "loading " << object_type::type_id << ": " << boost::core::demangle(typeid(object_type).name())
                  << std::endl;

        size_t sz = 0;
        fc::raw::unpack(_fstream, sz);
        if (sz > 0)
        {
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

            for (size_t ci = 0; ci < sz; ++ci)
            {
                object_id_type obj_id;
                fc::raw::unpack(_fstream, obj_id);
                const object_type* pobj = _state.template find<object_type>(obj_id);
                if (pobj == nullptr)
                {
                    _state.template create<object_type>([&](object_type& obj) {
                        fc::raw::unpack(_fstream, obj);
                        //                        fc::variant vo;
                        //                        fc::to_variant(obj, vo);
                        //                        std::cerr << "created " <<
                        //                        boost::core::demangle(typeid(object_type).name()) << ": "
                        //                                  << fc::json::to_pretty_string(vo) << std::endl;
                    });
                }
                else
                {
                    _state.template modify<object_type>(*pobj, [&](object_type& obj) {
                        fc::raw::unpack(_fstream, obj);
                        //                        fc::variant vo;
                        //                        fc::to_variant(obj, vo);
                        //                        std::cerr << "updated " <<
                        //                        boost::core::demangle(typeid(object_type).name()) << ":"
                        //                                  << fc::json::to_pretty_string(vo) << std::endl;
                    });
                }
            }
        }
    }

private:
    std::ifstream& _fstream;
    db_state& _state;
};

template <class Section>
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
            v.visit(load_index_visitor<Section>(fstream, state));
            loaded_idxs.insert(index_id);
        }
    });
}
}
}
