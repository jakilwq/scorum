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
#include <map>
#include <fstream>

#include <fc/crypto/ripemd160.hpp>

namespace pack_unpack_reflected_obj_tests {

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

using test_objects_type = std::vector<test_object_type>;

BOOST_AUTO_TEST_SUITE(pack_unpack_reflected_obj_tests)

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
    static const size_t common_buff_sz = 1024u;
    char common_buff[common_buff_sz];
    memset(common_buff, 0, common_buff_sz);

    test_objects_type vIn;

    vIn.push_back({ "Object ", 1, "T1" });
    vIn.push_back({ "ObjectObject ", 2, "T2" });
    vIn.push_back({ "ObjectObjectObject ", 3, "T3" });

    {
        fc::datastream<char*> ds_in(common_buff, common_buff_sz);

        fc::raw::pack(ds_in, vIn.size());
        for (const auto& val : vIn)
        {
            fc::raw::pack(ds_in, val);
        }
    }

    // std::cerr << fc::to_hex(common_buff, common_buff_sz) << std::endl;

    test_objects_type vOut;

    vOut.reserve(vIn.size());

    {
        fc::datastream<char*> ds_out(common_buff, common_buff_sz);

        size_t cn = 0;
        fc::raw::unpack(ds_out, cn);
        for (size_t ci = 0; ci < (size_t)cn; ++ci)
        {
            test_object_type objOut;
            fc::raw::unpack(ds_out, objOut);
            vOut.push_back(objOut);
        }
    }

    for (size_t ci = 0; ci < vOut.size(); ++ci)
    {
        BOOST_CHECK_EQUAL(vIn[ci].val0, vOut[ci].val0);
        BOOST_CHECK_EQUAL(vIn[ci].val1, vOut[ci].val1);
        BOOST_CHECK_EQUAL(vIn[ci].val2, vOut[ci].val2);
    }
}

BOOST_AUTO_TEST_CASE(vector_binary_save_and_load)
{
    test_objects_type vIn;

    vIn.push_back({ "Object ", 1, "T1" });
    vIn.push_back({ "ObjectObject ", 2, "T2" });
    vIn.push_back({ "ObjectObjectObject ", 3, "T3" });

    static const char* snapshot_file_path = "/tmp/test_snapshot.bin";

    fc::remove_all(snapshot_file_path);

    {
        std::ofstream snapshot_stream;
        snapshot_stream.open(snapshot_file_path, std::ios::binary);

        fc::ripemd160::encoder check_enc;
        fc::raw::pack(check_enc, __FUNCTION__);
        auto check = check_enc.result();

        fc::raw::pack(snapshot_stream, check);

        {
            fc::raw::pack(snapshot_stream, vIn.size());
            for (const auto& val : vIn)
            {
                fc::raw::pack(snapshot_stream, val);
            }
        }

        snapshot_stream.close();
    }

    test_objects_type vOut;

    vOut.reserve(vIn.size());

    {
        std::ifstream snapshot_stream;
        snapshot_stream.open(snapshot_file_path, std::ios::binary);

        {
            fc::ripemd160 check;

            fc::raw::unpack(snapshot_stream, check);

            fc::ripemd160::encoder check_enc;
            fc::raw::pack(check_enc, __FUNCTION__);

            BOOST_REQUIRE(check_enc.result() == check);

            size_t cn = 0;
            fc::raw::unpack(snapshot_stream, cn);
            for (size_t ci = 0; ci < (size_t)cn; ++ci)
            {
                test_object_type objOut;
                fc::raw::unpack(snapshot_stream, objOut);
                vOut.push_back(objOut);
            }
        }

        snapshot_stream.close();
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

FC_REFLECT(pack_unpack_reflected_obj_tests::test_object_type, (val0)(val1)(val2))
