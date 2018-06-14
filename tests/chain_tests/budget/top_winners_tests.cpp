#include <boost/test/unit_test.hpp>

#include "database_budget_integration.hpp"

#include <scorum/chain/services/dynamic_global_property.hpp>
#include <scorum/chain/services/dev_pool.hpp>
#include <scorum/chain/services/budget.hpp>

namespace top_winners_tests {

using namespace database_fixture;

struct top_winners_bundgets_fixture : public database_budget_integration_fixture
{
    top_winners_bundgets_fixture()
        : alice("alice")
        , bob("bob")
        , dynamic_global_property_service(db.dynamic_global_property_service())
        , dev_pool_service(db.dev_pool_service())
        , budget_service(db.budget_service())
    {
        open_database();

        auto amount = dev_pool_service.get().top_budgets_amount;
        BOOST_REQUIRE_GT(amount, 0u);
        budget_amount = ASSET_SCR(12e+3);
        int feed_amount = (budget_amount.amount * (amount + 10)).value;

        actor(initdelegate).create_account(alice);
        actor(initdelegate).give_scr(alice, feed_amount);
        actor(initdelegate).give_sp(alice, feed_amount);

        actor(initdelegate).create_account(bob);
        actor(initdelegate).give_scr(bob, feed_amount);
        actor(initdelegate).give_sp(bob, feed_amount);
    }

    asset budget_amount;

    Actor alice;
    Actor bob;

    dynamic_global_property_service_i& dynamic_global_property_service;
    dev_pool_service_i& dev_pool_service;
    budget_service_i& budget_service;
};

BOOST_FIXTURE_TEST_SUITE(top_winners_bundgets_tests, top_winners_bundgets_fixture)

BOOST_AUTO_TEST_CASE(get_monopoly_top_budgets_check)
{
    auto amount = dev_pool_service.get().top_budgets_amount;
    auto current_time = dynamic_global_property_service.get().time;
    auto alice_deadline_time = current_time + SCORUM_BLOCK_INTERVAL * 100;

    BOOST_REQUIRE_GT(amount, 0u);

    for (size_t ci = 0u; ci < amount; ++ci)
    {
        create_advertising_budget(alice, budget_amount, alice_deadline_time).push_in_block();
    }

    BOOST_REQUIRE_EQUAL(budget_service.get_budgets(alice.name).size(), amount);
    BOOST_REQUIRE_EQUAL(budget_service.get_top_budgets(amount).size(), amount);
}

BOOST_AUTO_TEST_CASE(get_concurrent_top_budgets_check)
{
    auto amount = dev_pool_service.get().top_budgets_amount;
    auto current_time = dynamic_global_property_service.get().time;
    auto deadline_step = SCORUM_BLOCK_INTERVAL * 100;
    auto alice_deadline_time = current_time + 2 * deadline_step;
    auto bob_deadline_time = alice_deadline_time + deadline_step;
    auto top_bob_deadline_time = alice_deadline_time - deadline_step;

    BOOST_REQUIRE_GT(amount, 0u);

    for (size_t ci = 0u; ci < amount; ++ci)
    {
        create_advertising_budget(alice, budget_amount, alice_deadline_time).push_in_block();
    }

    create_advertising_budget(bob, budget_amount, bob_deadline_time).push_in_block();
    create_advertising_budget(bob, budget_amount, bob_deadline_time).push_in_block();

    BOOST_REQUIRE_EQUAL(budget_service.get_top_budgets(amount).size(), amount);

    // alice is winner
    for (const budget_object& budget : budget_service.get_top_budgets(amount))
    {
        BOOST_CHECK_EQUAL((std::string)budget.owner, alice.name);
    }

    for (size_t ci = 0u; ci < amount; ++ci)
    {
        create_advertising_budget(bob, budget_amount, top_bob_deadline_time).push_in_block();
    }

    // bob became winner
    for (const budget_object& budget : budget_service.get_top_budgets(amount))
    {
        BOOST_CHECK_EQUAL((std::string)budget.owner, bob.name);
    }
}

BOOST_AUTO_TEST_SUITE_END()
}
