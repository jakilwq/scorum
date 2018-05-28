#include <boost/test/unit_test.hpp>

#include "fc/reflect/reflect.hpp"
#include <fc/io/json.hpp>
#include <fc/crypto/hex.hpp>

#include <fc/crypto/base58.hpp>
#include <fc/crypto/ripemd160.hpp>
#include <fc/exception/exception.hpp>
#include <fc/io/raw.hpp>

#include <string>
#include <iostream>
#include <sstream>
#include <vector>

namespace reflact_tests {

struct test_object_type
{
    test_object_type()
    {
    }
    test_object_type(const std::string& v0, int v1, const std::string& v2)
        : val0(v0)
        , val1(v1)
        , val2(v2)
    {
    }
    std::string val0;
    int val1;
    std::string val2;
};

BOOST_AUTO_TEST_SUITE(reflect_tests)

BOOST_AUTO_TEST_CASE(binary_reflect)
{
    test_object_type objIn{ "Object ", 1, "T1" };

    auto buff = fc::raw::pack(objIn);

    // std::cerr << fc::to_hex(buff) << std::endl;

    test_object_type objOut;

    fc::raw::unpack<test_object_type>(buff, objOut);

    BOOST_CHECK_EQUAL(objIn.val0, objOut.val0);
    BOOST_CHECK_EQUAL(objIn.val1, objOut.val1);
    BOOST_CHECK_EQUAL(objIn.val2, objOut.val2);
}

BOOST_AUTO_TEST_CASE(vector_binary_reflect)
{
    using test_objects_type = std::vector<test_object_type>;

    static const size_t common_buff_sz = 1024u;
    char common_buff[common_buff_sz];
    memset(common_buff, 0, common_buff_sz);

    test_objects_type vIn;

    vIn.push_back({ "Object ", 1, "T1" });
    vIn.push_back({ "ObjectObject ", 2, "T2" });
    vIn.push_back({ "ObjectObjectObject ", 3, "T3" });

    char* pbuff = common_buff;
    size_t sz = vIn.size();
    memcpy(pbuff, &sz, sizeof(sz));
    pbuff += sizeof(sz);

    for (const auto& val : vIn)
    {
        auto buff = fc::raw::pack(val);
        sz = buff.size();
        memcpy(pbuff, &sz, sizeof(sz));
        pbuff += sizeof(sz);
        memcpy(pbuff, &buff.front(), sz);
        pbuff += sz;
    }

    // std::cerr << fc::to_hex(common_buff, common_buff_sz) << std::endl;

    test_objects_type vOut;

    vOut.reserve(vIn.size());

    pbuff = common_buff;
    size_t cn = (*reinterpret_cast<size_t*>(pbuff));
    pbuff += sizeof(cn);

    for (size_t ci = 0; ci < cn; ++ci)
    {
        test_object_type objOut;

        size_t sz = (*reinterpret_cast<size_t*>(pbuff));
        pbuff += sizeof(sz);
        std::vector<char> buff(sz);
        memcpy(&buff.front(), pbuff, sz);
        fc::raw::unpack<test_object_type>(buff, objOut);
        vOut.push_back(objOut);
        pbuff += sz;
    }

    for (size_t ci = 0; ci < vOut.size(); ++ci)
    {
        BOOST_CHECK_EQUAL(vIn[ci].val0, vOut[ci].val0);
        BOOST_CHECK_EQUAL(vIn[ci].val1, vOut[ci].val1);
        BOOST_CHECK_EQUAL(vIn[ci].val2, vOut[ci].val2);
    }
}

BOOST_AUTO_TEST_SUITE_END()
}

FC_REFLECT(reflact_tests::test_object_type, (val0)(val1)(val2))
