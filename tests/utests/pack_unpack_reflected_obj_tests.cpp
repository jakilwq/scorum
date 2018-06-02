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
#include <fc/shared_buffer.hpp>

namespace fc {
namespace raw {
template <typename Stream, typename Val> Stream& pack(Stream& stream, const boost::container::vector<Val>& vec)
{
    size_t sz = vec.size();
    stream << sz;
    for (size_t ci = 0; ci < sz; ++ci)
    {
        stream << (*vec.nth(ci));
    }
    return stream;
}

template <typename Stream, typename Val> Stream& unpack(Stream& stream, boost::container::vector<Val>& vec)
{
    size_t sz = 0;
    stream >> sz;
    vec.reserve(sz);
    for (size_t ci = 0; ci < sz; ++ci)
    {
        Val v;
        stream >> v;
        vec.push_back(v);
    }
    return stream;
}
}
}

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
    test_object_type obj_in{ "Object ", 1, "T1" };

    auto buff = fc::raw::pack(obj_in);

    // std::cerr << fc::to_hex(buff) << std::endl;

    test_object_type obj_out;

    fc::raw::unpack<test_object_type>(buff, obj_out);

    BOOST_CHECK_EQUAL(obj_in.val0, obj_out.val0);
    BOOST_CHECK_EQUAL(obj_in.val1, obj_out.val1);
    BOOST_CHECK_EQUAL(obj_in.val2, obj_out.val2);
}

BOOST_AUTO_TEST_CASE(vector_binary_reflect)
{
    static const size_t common_buff_sz = 1024u;
    char common_buff[common_buff_sz];
    memset(common_buff, 0, common_buff_sz);

    test_objects_type v_obj_in;

    v_obj_in.push_back({ "Object ", 1, "T1" });
    v_obj_in.push_back({ "ObjectObject ", 2, "T2" });
    v_obj_in.push_back({ "ObjectObjectObject ", 3, "T3" });

    {
        fc::datastream<char*> ds_in(common_buff, common_buff_sz);

        fc::raw::pack(ds_in, v_obj_in.size());
        for (const auto& val : v_obj_in)
        {
            fc::raw::pack(ds_in, val);
        }
    }

    // std::cerr << fc::to_hex(common_buff, common_buff_sz) << std::endl;

    test_objects_type v_obj_out;

    v_obj_out.reserve(v_obj_in.size());

    {
        fc::datastream<char*> ds_out(common_buff, common_buff_sz);

        size_t cn = 0;
        fc::raw::unpack(ds_out, cn);
        for (size_t ci = 0; ci < (size_t)cn; ++ci)
        {
            test_object_type obj_out;
            fc::raw::unpack(ds_out, obj_out);
            v_obj_out.push_back(obj_out);
        }
    }

    BOOST_REQUIRE_EQUAL(v_obj_in.size(), v_obj_out.size());
    for (size_t ci = 0; ci < v_obj_out.size(); ++ci)
    {
        BOOST_CHECK_EQUAL(v_obj_in[ci].val0, v_obj_out[ci].val0);
        BOOST_CHECK_EQUAL(v_obj_in[ci].val1, v_obj_out[ci].val1);
        BOOST_CHECK_EQUAL(v_obj_in[ci].val2, v_obj_out[ci].val2);
    }
}

BOOST_AUTO_TEST_CASE(vector_binary_save_and_load)
{
    test_objects_type v_obj_in;

    v_obj_in.push_back({ "Object ", 1, "T1" });
    v_obj_in.push_back({ "ObjectObject ", 2, "T2" });
    v_obj_in.push_back({ "ObjectObjectObject ", 3, "T3" });

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
            fc::raw::pack(snapshot_stream, v_obj_in.size());
            for (const auto& val : v_obj_in)
            {
                fc::raw::pack(snapshot_stream, val);
            }
        }

        snapshot_stream.close();
    }

    test_objects_type v_obj_out;

    v_obj_out.reserve(v_obj_in.size());

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
                test_object_type obj_out;
                fc::raw::unpack(snapshot_stream, obj_out);
                v_obj_out.push_back(obj_out);
            }
        }

        snapshot_stream.close();
    }

    BOOST_REQUIRE_EQUAL(v_obj_in.size(), v_obj_out.size());
    for (size_t ci = 0; ci < v_obj_out.size(); ++ci)
    {
        BOOST_CHECK_EQUAL(v_obj_in[ci].val0, v_obj_out[ci].val0);
        BOOST_CHECK_EQUAL(v_obj_in[ci].val1, v_obj_out[ci].val1);
        BOOST_CHECK_EQUAL(v_obj_in[ci].val2, v_obj_out[ci].val2);
    }
}

BOOST_AUTO_TEST_CASE(shared_buffer_save_and_load)
{
    using shared_buffer = boost::container::vector<char>;
    shared_buffer v_buf_in;

    v_buf_in.push_back(0);
    v_buf_in.push_back(0);
    v_buf_in.push_back('a');
    v_buf_in.push_back('b');
    v_buf_in.push_back(0);

    static const char* snapshot_file_path = "/tmp/test_snapshot.bin";

    fc::remove_all(snapshot_file_path);

    {
        std::ofstream snapshot_stream;
        snapshot_stream.open(snapshot_file_path, std::ios::binary);

        fc::raw::pack(snapshot_stream, v_buf_in);

        snapshot_stream.close();
    }

    shared_buffer v_buf_out;

    v_buf_out.reserve(v_buf_in.size());

    {
        std::ifstream snapshot_stream;
        snapshot_stream.open(snapshot_file_path, std::ios::binary);

        fc::raw::unpack(snapshot_stream, v_buf_out);

        snapshot_stream.close();
    }

    BOOST_REQUIRE_EQUAL(v_buf_in.size(), v_buf_out.size());
    for (size_t ci = 0; ci < v_buf_out.size(); ++ci)
    {
        BOOST_CHECK_EQUAL(v_buf_in[ci], v_buf_out[ci]);
    }
}
BOOST_AUTO_TEST_SUITE_END()
}

FC_REFLECT(pack_unpack_reflected_obj_tests::test_object_type, (val0)(val1)(val2))
