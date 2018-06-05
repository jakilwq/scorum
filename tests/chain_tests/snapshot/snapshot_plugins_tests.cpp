#include <boost/test/unit_test.hpp>

#include "database_trx_integration.hpp"

#include "actor.hpp"

#include <scorum/chain/database/scheduled_snapshot.hpp>
#include <scorum/chain/services/dynamic_global_property.hpp>
#include <scorum/chain/services/snapshot.hpp>
#include <scorum/chain/services/account.hpp>

#include <scorum/blockchain_history/blockchain_history_plugin.hpp>

#include <graphene/utilities/tempdir.hpp>
#include <fc/filesystem.hpp>

namespace snapshot_plugins_tests {

using namespace scorum::chain;

struct snapshot_plugins_fixture : public database_fixture::database_trx_integration_fixture
{
    snapshot_plugins_fixture()
        : sam("sam")
        , dprops_service(db.dynamic_global_property_service())
        , snapshot_service(db.snapshot_service())
        , account_service(db.account_service())
    {
    }

    Actor sam;

    const int feed_amount = 1000;

    dynamic_global_property_service_i& dprops_service;
    snapshot_service_i& snapshot_service;
    account_service_i& account_service;
};

BOOST_FIXTURE_TEST_SUITE(snapshot_plugins_tests, snapshot_plugins_fixture)

BOOST_AUTO_TEST_CASE(saved_from_set1_and_loaded_to_set2_of_plugins)
{
    using namespace scorum::chain::database_ns;

    init_plugin<scorum::blockchain_history::blockchain_history_plugin>();
    open_database();

    generate_blocks(5);

    actor(initdelegate).create_account(sam);
    actor(initdelegate).give_scr(sam, feed_amount);
    actor(initdelegate).give_sp(sam, feed_amount);

    generate_block();

    fc::path dir = fc::temp_directory(graphene::utilities::temp_directory_path()).path();
    db.set_snapshot_dir(dir);

    db.schedule_snapshot_task();
    generate_block();

    //    fc::path snapshot_file
    //        = save_scheduled_snapshot::get_snapshot_path(dprops_service, snapshot_service.get_snapshot_dir());

    close_database();

    open_database();

    // TODO
}

BOOST_AUTO_TEST_SUITE_END()
}
