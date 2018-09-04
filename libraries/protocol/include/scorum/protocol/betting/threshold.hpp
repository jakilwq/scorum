#pragma once
#include <fc/reflect/reflect.hpp>

namespace scorum {
namespace protocol {
namespace betting {

struct threshold_type
{
    using value_type = int16_t;

    static constexpr value_type factor = 1000;

    value_type value;

    threshold_type(const value_type value)
        : value(value)
    {
    }
};

inline bool operator<(const threshold_type& lhs, threshold_type::value_type value)
{
    return lhs.value < threshold_type::factor * value;
}
inline bool operator<(threshold_type::value_type value, const threshold_type& rhs)
{
    return threshold_type::factor * value < rhs.value;
}
inline bool operator>(const threshold_type& lhs, threshold_type::value_type value)
{
    return lhs.value > threshold_type::factor * value;
}
inline bool operator>(threshold_type::value_type value, const threshold_type& rhs)
{
    return threshold_type::factor * value > rhs.value;
}
inline bool operator<(const threshold_type& lhs, const threshold_type& rhs)
{
    return lhs.value < rhs.value;
}
inline bool operator>(const threshold_type& lhs, const threshold_type& rhs)
{
    return lhs.value > rhs.value;
}
}
}
}

FC_REFLECT(scorum::protocol::betting::threshold_type, (value))