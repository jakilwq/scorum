#include <boost/test/unit_test.hpp>

#include "database_trx_integration.hpp"

#include "actor.hpp"

#include <scorum/chain/database/scheduled_snapshot.hpp>
#include <scorum/chain/services/dynamic_global_property.hpp>
#include <scorum/chain/services/snapshot.hpp>
#include <scorum/chain/services/account.hpp>

#include <scorum/chain/schema/account_objects.hpp>

#include <scorum/blockchain_history/blockchain_history_plugin.hpp>

#include <graphene/utilities/tempdir.hpp>
#include <fc/filesystem.hpp>

namespace snapshot_plugins_tests {

using namespace scorum::chain;

struct snapshot_plugins_fixture_impl : public database_fixture::database_trx_integration_fixture
{
    snapshot_plugins_fixture_impl()
        : dprops_service(db.dynamic_global_property_service())
        , snapshot_service(db.snapshot_service())
        , account_service(db.account_service())
    {
    }

    dynamic_global_property_service_i& dprops_service;
    snapshot_service_i& snapshot_service;
    account_service_i& account_service;
};

struct snapshot_plugins_fixture
{
    snapshot_plugins_fixture()
        : sam("sam")
    {
        reset();
    }

    void reset()
    {
        fixture.reset(new snapshot_plugins_fixture_impl());
    }

    std::unique_ptr<snapshot_plugins_fixture_impl> fixture;

    Actor sam;

    const int feed_amount = 1000;
};

BOOST_FIXTURE_TEST_SUITE(snapshot_plugins_tests, snapshot_plugins_fixture)

BOOST_AUTO_TEST_CASE(saved_from_set1_and_loaded_to_set2_of_plugins)
{
    using namespace scorum::chain::database_ns;

    fixture->init_plugin<scorum::blockchain_history::blockchain_history_plugin>();
    fixture->open_database();

    auto sz_idx_with_plugin = fixture->db.get_indexes_size();

    fixture->generate_block();

    fixture->actor(fixture->initdelegate).create_account(sam);
    fixture->actor(fixture->initdelegate).give_scr(sam, feed_amount);
    fixture->actor(fixture->initdelegate).give_sp(sam, feed_amount);

    fixture->generate_block();

    fc::path dir = fc::temp_directory(graphene::utilities::temp_directory_path()).path();
    fixture->db.set_snapshot_dir(dir);
    fixture->db.schedule_snapshot_task();

    fc::path snapshot_file = save_scheduled_snapshot::get_snapshot_path(fixture->dprops_service,
                                                                        fixture->snapshot_service.get_snapshot_dir());

    fc::remove_all(snapshot_file);

    save_scheduled_snapshot saver(fixture->db);

    block_task_context ctx(static_cast<data_service_factory&>(fixture->db),
                           static_cast<database_virtual_operations_emmiter_i&>(fixture->db),
                           fixture->db.head_block_num());

    BOOST_REQUIRE_NO_THROW(saver.apply(ctx));

    reset();

    fixture->open_database();

    BOOST_REQUIRE_GT(sz_idx_with_plugin, fixture->db.get_indexes_size());

    load_scheduled_snapshot loader(fixture->db);

    BOOST_REQUIRE_NO_THROW(loader.load(snapshot_file));

    BOOST_REQUIRE_NO_THROW(fixture->account_service.check_account_existence(sam.name));

    const auto& loaded_account = fixture->account_service.get_account(sam.name);

    BOOST_REQUIRE_GE(loaded_account.scorumpower, ASSET_SP(feed_amount));
    BOOST_REQUIRE_EQUAL(loaded_account.balance, ASSET_SCR(feed_amount));
}

BOOST_AUTO_TEST_SUITE_END()
}
