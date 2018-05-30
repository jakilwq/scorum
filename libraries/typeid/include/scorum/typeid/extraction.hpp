#pragma once

#include "get_types.hpp"

#include <boost/preprocessor.hpp>

#include <fc/exception/exception.hpp>
#include <fc/static_variant.hpp>
#include <fc/reflect/reflect.hpp>
#include <fc/fixed_string.hpp>

#define SCORUM_OBJECT_TYPE_EXTRACTION_MAKE_TYPE(elem) get_object_type<BOOST_PP_CAT(elem, _object_type)>

#define SCORUM_OBJECT_TYPE_EXTRACTION_MAKE_TYPE_LIST(_1, _2, n, elem)                                                  \
    BOOST_PP_COMMA_IF(BOOST_PP_NOT_EQUAL(n, 0)) SCORUM_OBJECT_TYPE_EXTRACTION_MAKE_TYPE(elem)

#define SCORUM_OBJECT_TYPE_EXTRACTION_RETURN_TYPE(_1, enum_type, elem)                                                 \
    case enum_type::BOOST_PP_SEQ_CAT((elem)(_object_type)):                                                            \
        return object_type_variant_type(get_object_type<BOOST_PP_CAT(elem, _object_type)>());

// use this macro for object enumed ids to extract object type from id
#define SCORUM_OBJECT_TYPE_EXTRACTION(ENUM, FIELDS)                                                                    \
    namespace scorum {                                                                                                 \
    namespace {                                                                                                        \
    using object_type_variant_type                                                                                     \
        = fc::static_variant<BOOST_PP_SEQ_FOR_EACH_I(SCORUM_OBJECT_TYPE_EXTRACTION_MAKE_TYPE_LIST, _, FIELDS)>;        \
    struct get_extraction_section                                                                                      \
    {                                                                                                                  \
        fc::fixed_string<uint64_t> name = BOOST_PP_STRINGIZE(BOOST_PP_SEQ_ELEM(1, FIELDS));                            \
    };                                                                                                                 \
    inline object_type_variant_type get_object_type_variant(int id)                                                    \
    {                                                                                                                  \
        switch (id)                                                                                                    \
        {                                                                                                              \
            BOOST_PP_SEQ_FOR_EACH(SCORUM_OBJECT_TYPE_EXTRACTION_RETURN_TYPE, ENUM, FIELDS)                             \
        default:                                                                                                       \
            FC_THROW_EXCEPTION(fc::bad_cast_exception, "Can't find type for id = ${id}", ("id", id));                  \
        }                                                                                                              \
        return object_type_variant_type();                                                                             \
    }                                                                                                                  \
    }                                                                                                                  \
    }
