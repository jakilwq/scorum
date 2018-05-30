#pragma once

#include <fstream>
#include <chainbase/db_state.hpp>

#include <scorum/snapshot/data_struct_hash.hpp>

namespace scorum {
namespace snapshot {

using db_state = chainbase::db_state;

template <class IterationTag> class save_index_visitor
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
        using object_type = typename T::type;
        const auto& index = _state.template get_index<typename chainbase::get_index_type<object_type>::type>()
                                .indices()
                                .template get<IterationTag>();
        size_t sz = index.size();
        fc::raw::pack(_fstream, index.size());
        auto itr = index.begin();
        if (sz > 0)
        {
            fc::raw::pack(_fstream, get_data_struct_hash(*itr));
            for (; itr != index.end(); ++itr)
            {
                const object_type& obj = (*itr);
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

    state.for_each_index_key([&](int index_id) {
        auto v = section.get_object_type_variant(index_id);
        v.visit(save_index_visitor<IterationTag>(fstream, state));
    });
}
}
}
