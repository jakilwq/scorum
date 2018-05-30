#pragma once

#include <fstream>
#include <chainbase/db_state.hpp>

#include <scorum/snapshot/data_struct_hash.hpp>

namespace scorum {
namespace snapshot {

using db_state = chainbase::db_state;

class load_index_visitor
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
        FC_ASSERT(_state.template find<object_type>() == nullptr, "State is not empty");
        size_t sz = 0;
        fc::raw::unpack(_fstream, sz);
        if (sz > 0)
        {
            fc::ripemd160 check, etalon;

            _state.template create<object_type>([&](object_type& obj) { etalon = get_data_struct_hash(obj); });
            fc::raw::unpack(_fstream, check);

            FC_ASSERT(check == etalon);

            _state.template remove(*_state.template find<object_type>());

            for (size_t ci = 0; ci < sz; ++ci)
            {
                _state.template create<object_type>([&](object_type& obj) { fc::raw::unpack(_fstream, obj); });
            }
        }
    }

private:
    std::ifstream& _fstream;
    db_state& _state;
};

template <class Section>
void load_index_section(std::ifstream& fstream, chainbase::db_state& state, const Section& section)
{
    fc::ripemd160 check;

    fc::raw::unpack(fstream, check);

    fc::ripemd160::encoder check_enc;
    fc::raw::pack(check_enc, section.name);

    FC_ASSERT(check_enc.result() == check);

    state.for_each_index_key([&](int index_id) {
        auto v = section.get_object_type_variant(index_id);
        v.visit(load_index_visitor(fstream, state));
    });
}
}
}
