#include <boost/test/unit_test.hpp>

#include "database_trx_integration.hpp"

#include <scorum/chain/database/scheduled_snapshot.hpp>
#include <scorum/chain/services/dynamic_global_property.hpp>
#include <scorum/chain/services/snapshot.hpp>

#include <graphene/utilities/tempdir.hpp>
#include <fc/filesystem.hpp>

namespace snapshot_tests {

using namespace scorum::chain;

struct snapshot_fixture : public database_fixture::database_trx_integration_fixture
{
    snapshot_fixture()
        : dprops_service(db.dynamic_global_property_service())
        , snapshot_service(db.snapshot_service())
    {
        open_database();

        db.save_snapshot.connect([&](std::ofstream& fs) { save_plugin_snapshot(fs); });

        generate_blocks(2);
    }

    void save_plugin_snapshot(std::ofstream&)
    {
        snapshot_saved_for_plugin = true;
    }

    fc::path get_snapshot_path(const fc::path& snapshot_dir)
    {
        std::stringstream snapshot_name;
        snapshot_name << dprops_service.get().time.to_iso_string();
        snapshot_name << "-";
        snapshot_name << dprops_service.get().head_block_number;
        snapshot_name << ".bin";

        return snapshot_dir / snapshot_name.str();
    }

    bool snapshot_saved_for_plugin = false;
    dynamic_global_property_service_i& dprops_service;
    snapshot_service_i& snapshot_service;
};

BOOST_FIXTURE_TEST_SUITE(snapshot_tests, snapshot_fixture)

BOOST_AUTO_TEST_CASE(dont_make_snapshot_with_no_snapshot_dir)
{
    db.schedule_snapshot_task();

    BOOST_CHECK_EQUAL(snapshot_service.is_snapshot_scheduled(), false);

    generate_block();

    BOOST_CHECK_EQUAL(snapshot_saved_for_plugin, false);
}

// BOOST_AUTO_TEST_CASE(make_snapshot_with_snapshot_dir)
//{
//    fc::path dir = fc::temp_directory(graphene::utilities::temp_directory_path()).path();
//    db.set_snapshot_dir(dir);
//    db.schedule_snapshot_task();

//    BOOST_CHECK_EQUAL(snapshot_service.is_snapshot_scheduled(), true);

//    generate_block();

//    BOOST_CHECK_EQUAL(snapshot_saved_for_plugin, true);

//    BOOST_CHECK_EQUAL(snapshot_service.is_snapshot_scheduled(), false);
//}

BOOST_AUTO_TEST_CASE(save_and_load_snapshot_for_genesis_state)
{
    using namespace scorum::chain::database_ns;

    fc::path dir = fc::temp_directory(graphene::utilities::temp_directory_path()).path();
    db.set_snapshot_dir(dir);
    db.schedule_snapshot_task();

    BOOST_CHECK_EQUAL(snapshot_service.is_snapshot_scheduled(), true);

    fc::path snapshot_file = get_snapshot_path(snapshot_service.get_snapshot_dir());

    fc::remove_all(snapshot_file);

    save_scheduled_snapshot saver(db);

    block_task_context ctx(static_cast<data_service_factory&>(db),
                           static_cast<database_virtual_operations_emmiter_i&>(db), db.head_block_num());

    BOOST_REQUIRE_NO_THROW(saver.apply(ctx));

    BOOST_REQUIRE(fc::exists(snapshot_file));

    load_scheduled_snapshot loader(db);

    snapshot_header header = loader.load_header(snapshot_file);

    BOOST_REQUIRE_EQUAL(header.head_block_number, dprops_service.get().head_block_number);
    BOOST_REQUIRE_EQUAL(header.chainbase_flags, chainbase::database::read_write);

    loader.load(snapshot_file);

    // TODO
}

BOOST_AUTO_TEST_SUITE_END()
}
