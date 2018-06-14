#include <boost/test/unit_test.hpp>

#include "database_budget_integration.hpp"

#include <scorum/chain/services/dynamic_global_property.hpp>
#include <scorum/chain/services/dev_pool.hpp>
#include <scorum/chain/services/budget.hpp>

#include <scorum/chain/operation_notification.hpp>

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

        max_top_amount = dev_pool_service.get().top_budgets_amount;
        BOOST_REQUIRE_GT(max_top_amount, 0u);
        budget_amount = ASSET_SCR(12e+3);
        int feed_amount = (budget_amount.amount * (max_top_amount + 10)).value;

        actor(initdelegate).create_account(alice);
        actor(initdelegate).give_scr(alice, feed_amount);
        actor(initdelegate).give_sp(alice, feed_amount);

        actor(initdelegate).create_account(bob);
        actor(initdelegate).give_scr(bob, feed_amount);
        actor(initdelegate).give_sp(bob, feed_amount);

        auto current_time = dynamic_global_property_service.get().time;
        auto deadline_step = SCORUM_BLOCK_INTERVAL * 100;
        alice_deadline_time = current_time + 2 * deadline_step;
        bob_deadline_time = alice_deadline_time + deadline_step;
        top_bob_deadline_time = alice_deadline_time - deadline_step;
    }

    void fill_top_with_actor(Actor& actor, const fc::time_point_sec& actor_deadline_time)
    {
        for (size_t ci = 0u; ci < max_top_amount; ++ci)
        {
            create_advertising_budget(actor, budget_amount, actor_deadline_time).push_in_block();
        }
    }

    uint16_t max_top_amount = 0u;
    fc::time_point_sec alice_deadline_time;
    fc::time_point_sec bob_deadline_time;
    fc::time_point_sec top_bob_deadline_time;

    asset budget_amount;

    Actor alice;
    Actor bob;

    dynamic_global_property_service_i& dynamic_global_property_service;
    dev_pool_service_i& dev_pool_service;
    budget_service_i& budget_service;
};

struct advertising_bundgets_visitor
{
    typedef void result_type;

    advertising_bundgets_visitor(Actor& checked_actor)
        : _checked_actor(checked_actor)
    {
    }

    void operator()(const allocate_cash_from_advertising_budget_operation& op) const
    {
        BOOST_CHECK_EQUAL((std::string)_checked_actor.name, op.owner);
    }

    template <typename Op> void operator()(Op&&) const
    {
    }

private:
    Actor& _checked_actor;
};

BOOST_FIXTURE_TEST_SUITE(top_winners_bundgets_tests, top_winners_bundgets_fixture)

BOOST_AUTO_TEST_CASE(get_monopoly_top_budgets_check)
{
    fill_top_with_actor(alice, alice_deadline_time);

    BOOST_REQUIRE_EQUAL(budget_service.get_budgets(alice.name).size(), max_top_amount);
    BOOST_REQUIRE_EQUAL(budget_service.get_top_budgets(max_top_amount).size(), max_top_amount);
}

BOOST_AUTO_TEST_CASE(get_concurrent_top_budgets_check)
{
    fill_top_with_actor(alice, alice_deadline_time);

    create_advertising_budget(bob, budget_amount, bob_deadline_time).push_in_block();
    create_advertising_budget(bob, budget_amount, bob_deadline_time).push_in_block();

    BOOST_REQUIRE_EQUAL(budget_service.get_top_budgets(max_top_amount).size(), max_top_amount);

    // alice is winner
    for (const budget_object& budget : budget_service.get_top_budgets(max_top_amount))
    {
        BOOST_CHECK_EQUAL((std::string)budget.owner, alice.name);
    }

    fill_top_with_actor(bob, top_bob_deadline_time);

    // bob became winner
    for (const budget_object& budget : budget_service.get_top_budgets(max_top_amount))
    {
        BOOST_CHECK_EQUAL((std::string)budget.owner, bob.name);
    }
}

BOOST_AUTO_TEST_CASE(allocation_from_top_budgets_check)
{
    {
        fill_top_with_actor(alice, alice_deadline_time);

        create_advertising_budget(bob, budget_amount, bob_deadline_time).push_in_block();
        create_advertising_budget(bob, budget_amount, bob_deadline_time).push_in_block();

        // clang-format off
        boost::signals2::scoped_connection conn = db.post_apply_operation.connect(
            [&](const operation_notification& note)
        {
            note.op.visit(advertising_bundgets_visitor(alice));
        });
        // clang-format on

        generate_blocks(5);
    }

    {
        fill_top_with_actor(bob, top_bob_deadline_time);

        boost::signals2::scoped_connection conn = db.post_apply_operation.connect(
            [&](const operation_notification& note) { note.op.visit(advertising_bundgets_visitor(bob)); });

        generate_blocks(5);
    }
}

BOOST_AUTO_TEST_SUITE_END()
}
