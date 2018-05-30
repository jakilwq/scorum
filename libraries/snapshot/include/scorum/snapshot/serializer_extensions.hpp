#pragma once

#include <fc/shared_containers.hpp>
#include <scorum/protocol/types.hpp>

// These operators are used by pack/unpack methods
//
namespace fc {

template <typename Stream, typename ObjectType> Stream& operator<<(Stream& stream, const chainbase::oid<ObjectType>& id)
{
    stream << id._id;
    return stream;
}

template <typename Stream, typename ObjectType> Stream& operator>>(Stream& stream, chainbase::oid<ObjectType>& id)
{
    stream >> id._id;
    return stream;
}

using scorum::protocol::account_name_type;

using account_name_flat_set_type = fc::shared_flat_set<account_name_type>;

template <typename Stream> Stream& operator<<(Stream& stream, const account_name_flat_set_type& fs)
{
    size_t sz = fs.size();
    stream << sz;
    for (size_t ci = 0; ci < sz; ++ci)
    {
        stream << (*fs.nth(ci));
    }
    return stream;
}

template <typename Stream> Stream& operator>>(Stream& stream, account_name_flat_set_type& fs)
{
    size_t sz = 0;
    stream >> sz;
    for (size_t ci = 0; ci < sz; ++ci)
    {
        account_name_type a;
        stream >> a;
        fs.insert(a);
    }
    return stream;
}
}
