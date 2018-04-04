#ifdef IS_TEST_NET
#include <boost/test/unit_test.hpp>

#include <scorum/protocol/exceptions.hpp>

#include <scorum/chain/schema/block_summary_object.hpp>
#include <scorum/chain/database/database.hpp>
#include <scorum/chain/hardfork.hpp>
#include <scorum/blockchain_history/schema/operation_objects.hpp>
#include <scorum/chain/schema/scorum_objects.hpp>
#include <scorum/chain/services/account.hpp>
#include <scorum/chain/services/comment.hpp>
#include <scorum/chain/services/reward_fund.hpp>
#include <scorum/chain/services/dynamic_global_property.hpp>

#include <scorum/chain/util/reward.hpp>

#include <scorum/plugins/debug_node/debug_node_plugin.hpp>

#include <fc/crypto/digest.hpp>
#include "database_trx_integration.hpp"

#include <cmath>

using namespace scorum;
using namespace scorum::chain;
using namespace scorum::protocol;
using namespace scorum::app;

namespace database_fixture {
struct reward_fund_integration_fixture : public database_trx_integration_fixture
{
    const asset account_initial_vest_supply = ASSET_SP(100e+9); // 100.0 SP

    reward_fund_integration_fixture()
        : alice("alice")
        , bob("bob")
    {
        // switch off registration pool to get 0 SP for both alice and bob acounts
        genesis_state_type genesis = Genesis::create()
                                         .accounts_supply(TEST_ACCOUNTS_INITIAL_SUPPLY)
                                         .rewards_supply(ASSET_SCR(4800000e+9))
                                         .witnesses(initdelegate)
                                         .accounts(alice, bob)
                                         .dev_committee(initdelegate)
                                         .generate();
        open_database(genesis);

        actor(initdelegate).give_sp(alice, account_initial_vest_supply.amount.value);
        actor(initdelegate).give_sp(bob, account_initial_vest_supply.amount.value);

        generate_block();
    }

    Actor alice;
    Actor bob;
};

struct reward_fund_decay_fixture : public reward_fund_integration_fixture
{
    reward_fund_decay_fixture()
        : sam("sam")
    {
        actor(initdelegate).create_account(sam);
        actor(initdelegate).give_sp(sam, account_initial_vest_supply.amount.value);

        generate_block();
    }

    const std::string post_permlink = "alice-test";

    fc::time_point_sec post()
    {
        comment_operation comment;

        comment.author = alice.name;
        comment.permlink = post_permlink;
        comment.parent_permlink = "posts";
        comment.title = "foo";
        comment.body = "bar";

        push_operation_only(comment, alice.private_key);

        return db.obtain_service<dbs_comment>().get(alice.name, post_permlink).cashout_time;
    }

    const std::string comment_permlink = "bob-test";

    fc::time_point_sec comment()
    {
        comment_operation comment;

        comment.author = bob.name;
        comment.permlink = comment_permlink;
        comment.parent_author = alice.name;
        comment.parent_permlink = post_permlink;
        comment.title = "re: foo";
        comment.body = "re: bar";

        push_operation_only(comment, bob.private_key);

        return db.obtain_service<dbs_comment>().get(bob.name, comment_permlink).cashout_time;
    }

    void vote_for_post()
    {
        vote_operation vote;

        vote.voter = sam.name;
        vote.author = alice.name;
        vote.permlink = post_permlink;
        vote.weight = (int16_t)100;

        push_operation_only(vote, sam.private_key);
    }

    void vote_for_comment()
    {
        vote_operation vote;

        vote.voter = sam.name;
        vote.author = bob.name;
        vote.permlink = comment_permlink;
        vote.weight = (int16_t)100;

        push_operation_only(vote, sam.private_key);
    }

    Actor sam;
};
}

BOOST_AUTO_TEST_SUITE(reward_fund_tests)

BOOST_FIXTURE_TEST_CASE(reward_fund, database_fixture::reward_fund_integration_fixture)
{
    try
    {
        BOOST_TEST_MESSAGE("Testing: reward_fund");

        const auto blocks_between_comments = 5;

        BOOST_REQUIRE_EQUAL(db.obtain_service<dbs_account>().get_account("alice").balance, asset(0, SCORUM_SYMBOL));
        BOOST_REQUIRE_EQUAL(db.obtain_service<dbs_account>().get_account("bob").balance, asset(0, SCORUM_SYMBOL));
        BOOST_REQUIRE_EQUAL(db.obtain_service<dbs_account>().get_account("alice").scorumpower,
                            account_initial_vest_supply);
        BOOST_REQUIRE_EQUAL(db.obtain_service<dbs_account>().get_account("bob").scorumpower,
                            account_initial_vest_supply);

        comment_operation comment;
        vote_operation vote;
        signed_transaction tx;

        comment.author = "alice";
        comment.permlink = "test";
        comment.parent_permlink = "test";
        comment.title = "foo";
        comment.body = "bar";
        vote.voter = "alice";
        vote.author = "alice";
        vote.permlink = "test";
        vote.weight = (int16_t)100;
        tx.operations.push_back(comment);
        tx.operations.push_back(vote);
        tx.set_expiration(db.head_block_time() + SCORUM_MAX_TIME_UNTIL_EXPIRATION);
        tx.sign(alice.private_key, db.get_chain_id());
        db.push_transaction(tx, 0);

        generate_blocks(blocks_between_comments);

        comment.author = "bob";
        comment.parent_author = "alice";
        vote.voter = "bob";
        vote.author = "bob";
        tx.clear();
        tx.operations.push_back(comment);
        tx.operations.push_back(vote);
        tx.sign(bob.private_key, db.get_chain_id());
        db.push_transaction(tx, 0);

        const auto& fund = db.obtain_service<dbs_reward_fund>().get();

        BOOST_REQUIRE_GT(fund.activity_reward_balance_scr, asset(0, SCORUM_SYMBOL));
        BOOST_REQUIRE_EQUAL(fund.recent_claims.to_uint64(), uint64_t(0));

        share_type alice_comment_net_rshares
            = db.obtain_service<dbs_comment>().get("alice", std::string("test")).net_rshares;
        share_type bob_comment_net_rshares
            = db.obtain_service<dbs_comment>().get("bob", std::string("test")).net_rshares;

        {
            generate_blocks(db.obtain_service<dbs_comment>().get("alice", std::string("test")).cashout_time);

            BOOST_REQUIRE_EQUAL(fund.activity_reward_balance_scr, ASSET_SCR(0));
            BOOST_REQUIRE_EQUAL(fund.recent_claims.to_uint64(), alice_comment_net_rshares);

            // clang-format off
            BOOST_REQUIRE_GT   (db.obtain_service<dbs_account>().get_account("alice").scorumpower, account_initial_vest_supply);
            BOOST_REQUIRE_EQUAL(db.obtain_service<dbs_account>().get_account("alice").scorumpower,
                                account_initial_vest_supply + ASSET_SP(db.obtain_service<dbs_account>().get_account("alice").balance.amount.value));

            BOOST_REQUIRE_EQUAL(db.obtain_service<dbs_account>().get_account("bob").scorumpower, account_initial_vest_supply);
            BOOST_REQUIRE_EQUAL(db.obtain_service<dbs_account>().get_account("bob").scorumpower,
                                account_initial_vest_supply + ASSET_SP(db.obtain_service<dbs_account>().get_account("bob").balance.amount.value));
            // clang-format on

            validate_database();
        }

        {
            generate_blocks(blocks_between_comments);

            for (auto i = 0; i < blocks_between_comments; ++i)
            {
                share_type decay
                    = alice_comment_net_rshares * SCORUM_BLOCK_INTERVAL / SCORUM_RECENT_RSHARES_DECAY_RATE.to_seconds();
                alice_comment_net_rshares -= decay;
            }

            BOOST_REQUIRE_EQUAL(fund.recent_claims.to_uint64(), alice_comment_net_rshares + bob_comment_net_rshares);
            BOOST_REQUIRE_GT(fund.activity_reward_balance_scr, ASSET_SCR(0));

            BOOST_REQUIRE_GT(db.obtain_service<dbs_account>().get_account("alice").scorumpower,
                             account_initial_vest_supply);
            BOOST_REQUIRE_GT(db.obtain_service<dbs_account>().get_account("bob").scorumpower,
                             account_initial_vest_supply);

            validate_database();
        }
    }
    FC_LOG_AND_RETHROW()
}

BOOST_FIXTURE_TEST_CASE(recent_claims_decay, database_fixture::reward_fund_integration_fixture)
{
    try
    {
        BOOST_TEST_MESSAGE("Testing: recent_rshares_2decay");

        comment_operation comment;
        vote_operation vote;
        signed_transaction tx;

        comment.author = "alice";
        comment.permlink = "test";
        comment.parent_permlink = "test";
        comment.title = "foo";
        comment.body = "bar";
        vote.voter = "alice";
        vote.author = "alice";
        vote.permlink = "test";
        vote.weight = (int16_t)100;
        tx.operations.push_back(comment);
        tx.operations.push_back(vote);
        tx.set_expiration(db.head_block_time() + SCORUM_MAX_TIME_UNTIL_EXPIRATION);
        tx.sign(alice.private_key, db.get_chain_id());
        db.push_transaction(tx, 0);

        auto alice_vshares = util::evaluate_reward_curve(
            db.obtain_service<dbs_comment>().get("alice", std::string("test")).net_rshares.value,
            db.obtain_service<dbs_reward_fund>().get().author_reward_curve);

        generate_blocks(5);

        comment.author = "bob";
        vote.voter = "bob";
        vote.author = "bob";
        tx.clear();
        tx.operations.push_back(comment);
        tx.operations.push_back(vote);
        tx.sign(bob.private_key, db.get_chain_id());
        db.push_transaction(tx, 0);

        generate_blocks(db.obtain_service<dbs_comment>().get("alice", std::string("test")).cashout_time);

        {
            const auto& post_rf = db.obtain_service<dbs_reward_fund>().get();

            BOOST_REQUIRE(post_rf.recent_claims == alice_vshares);
            validate_database();
        }

        auto bob_cashout_time = db.obtain_service<dbs_comment>().get("bob", std::string("test")).cashout_time;
        auto bob_vshares = util::evaluate_reward_curve(
            db.obtain_service<dbs_comment>().get("bob", std::string("test")).net_rshares.value,
            db.obtain_service<dbs_reward_fund>().get().author_reward_curve);

        while (db.head_block_time() < bob_cashout_time - SCORUM_BLOCK_INTERVAL)
        {
            generate_block();

            alice_vshares -= (alice_vshares * SCORUM_BLOCK_INTERVAL) / SCORUM_RECENT_RSHARES_DECAY_RATE.to_seconds();
            const auto& post_rf = db.obtain_service<dbs_reward_fund>().get();

            BOOST_REQUIRE(post_rf.recent_claims == alice_vshares);
        }

        generate_block();

        {
            alice_vshares -= (alice_vshares * SCORUM_BLOCK_INTERVAL) / SCORUM_RECENT_RSHARES_DECAY_RATE.to_seconds();
            const auto& post_rf = db.obtain_service<dbs_reward_fund>().get();

            BOOST_REQUIRE(post_rf.recent_claims == alice_vshares + bob_vshares);
            validate_database();
        }
    }
    FC_LOG_AND_RETHROW()
}

BOOST_FIXTURE_TEST_CASE(recent_claims_long_decay, database_fixture::reward_fund_decay_fixture)
{
    int ci = 0;
    while (ci++ < 4)
    {
        auto cashout_post = post();

        auto delta = (cashout_post - db.head_block_time()).to_seconds();

        dlog("${d}", ("d", delta));

        generate_blocks(fc::time_point_sec(db.head_block_time().sec_since_epoch() + delta / 3), false);

        auto cashout_comment = comment();

        delta = (cashout_comment - cashout_post).to_seconds();

        dlog("${d}", ("d", delta));

        generate_blocks(fc::time_point_sec(db.head_block_time().sec_since_epoch() + 2 * delta / 3), false);

        vote_for_post();
        vote_for_comment();

        // share_type alice_comment_net_rshares = db.obtain_service<dbs_comment>().get(alice.name,
        // post_permlink).net_rshares;
        // share_type bob_comment_net_rshares = db.obtain_service<dbs_comment>().get(bob.name,
        // comment_permlink).net_rshares;

        generate_blocks(cashout_post);

        generate_blocks(cashout_comment);
    }
}

BOOST_AUTO_TEST_SUITE_END()

#endif
