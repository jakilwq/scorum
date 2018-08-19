#include <boost/test/unit_test.hpp>

#include <boost/preprocessor/seq/for_each.hpp>

#include <fc/static_variant.hpp>
#include <fc/io/json.hpp>
//#include <scorum/protocol/betting/market2.hpp>

struct over
{
};
struct under
{
};
struct yes
{
};
struct no
{
};

template <typename First, typename Second> struct wincase_pair
{
    typedef First first_type;
    typedef Second second_type;
};

using over_under = wincase_pair<over, under>;
using yes_no = wincase_pair<yes, no>;

#define PAIR_TO_SEQ(PAIR) (PAIR::first_type)(PAIR::second_type)

#define EXTRACT_PAIR_TYPES(r, data, elem) PAIR_TO_SEQ(elem)

#define DECLARE_WINCASES(WINCASES)                                                                                     \
    BOOST_PP_SEQ_ENUM(BOOST_PP_SEQ_FOR_EACH(EXTRACT_PAIR_TYPES, BOOST_PP_SEQ_NIL, WINCASES))

using wincases = fc::static_variant<DECLARE_WINCASES((over_under)(yes_no))>;

void to_variant(const wincases& w, fc::variant& v)
{
}

void from_variant(const fc::variant& v, wincases& w)
{
}

BOOST_AUTO_TEST_CASE(xxx)
{
    wincases w = over();

    BOOST_CHECK_EQUAL(fc::json::to_string(w), "");
}

FC_REFLECT_TYPENAME(wincases)

FC_REFLECT(over, )
FC_REFLECT(under, )
FC_REFLECT(yes, )
FC_REFLECT(no, )

// using wincases = fc::static_variant<BOOST_PP_SEQ_ENUM(PAIR_TO_SEQ(over_under) PAIR_TO_SEQ(yes_no))>;
