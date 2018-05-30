#pragma once

#include <stdint.h>

namespace scorum {

/** this class is meant to be specified to enable lookup of index type by object type using
* the SET_INDEX_TYPE macro.
**/
template <typename T> struct get_index_type
{
};

template <uint16_t Id> struct get_object_type;
}

/**
*  This macro must be used at global scope and OBJECT_TYPE and INDEX_TYPE must be fully qualified
*/
#define CHAINBASE_SET_INDEX_TYPE(OBJECT_TYPE, INDEX_TYPE)                                                              \
    namespace scorum {                                                                                                 \
    template <> struct get_index_type<OBJECT_TYPE>                                                                     \
    {                                                                                                                  \
        typedef INDEX_TYPE type;                                                                                       \
    };                                                                                                                 \
    template <> struct get_object_type<OBJECT_TYPE::type_id>                                                           \
    {                                                                                                                  \
        typedef OBJECT_TYPE type;                                                                                      \
    };                                                                                                                 \
    }
