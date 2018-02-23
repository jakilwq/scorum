#include <scorum/chain/data_service_factory.hpp>

#include <scorum/chain/database.hpp>

#include <scorum/chain/services/atomicswap.hpp>
#include <scorum/chain/services/account.hpp>
#include <scorum/chain/services/budget.hpp>
#include <scorum/chain/services/comment.hpp>
#include <scorum/chain/services/comment_vote.hpp>
#include <scorum/chain/services/decline_voting_rights_request.hpp>
#include <scorum/chain/services/dynamic_global_property.hpp>
#include <scorum/chain/services/escrow.hpp>
#include <scorum/chain/services/proposal.hpp>
#include <scorum/chain/services/registration_committee.hpp>
#include <scorum/chain/services/registration_pool.hpp>
#include <scorum/chain/services/reward.hpp>
#include <scorum/chain/services/reward_fund.hpp>
#include <scorum/chain/services/vesting_delegation.hpp>
#include <scorum/chain/services/withdraw_vesting_route.hpp>
#include <scorum/chain/services/witness.hpp>
#include <scorum/chain/services/witness_vote.hpp>

// namespace scorum {
// namespace chain {

// data_service_factory::data_service_factory(scorum::chain::database& db)
//    : factory(db)
//{
//}

// data_service_factory::~data_service_factory()
//{
//}

// account_service_i data_service_factory::account_service()
//{
//    return factory.obtain_service<account_service_i>();
//}

// atomicswap_service_i data_service_factory::atomicswap_service()
//{
//    return factory.obtain_service<atomicswap_service_i>();
//}

// budget_service_i data_service_factory::budget_service()
//{
//    return factory.obtain_service<budget_service_i>();
//}

// comment_service_i data_service_factory::comment_service()
//{
//    return factory.obtain_service<comment_service_i>();
//}

// comment_vote_service_i data_service_factory::comment_vote_service()
//{
//    return factory.obtain_service<comment_vote_service_i>();
//}

// decline_voting_rights_request_service_i data_service_factory::decline_voting_rights_request_service()
//{
//    return factory.obtain_service<decline_voting_rights_request_service_i>();
//}

// dynamic_global_property_service_i data_service_factory::dynamic_global_property_service()
//{
//    return factory.obtain_service<dynamic_global_property_service_i>();
//}

// escrow_service_i data_service_factory::escrow_service()
//{
//    return factory.obtain_service<escrow_service_i>();
//}

// proposal_service_i data_service_factory::proposal_service()
//{
//    return factory.obtain_service<proposal_service_i>();
//}

// registration_committee_service_i data_service_factory::registration_committee_service()
//{
//    return factory.obtain_service<registration_committee_service_i>();
//}

// registration_pool_service_i data_service_factory::registration_pool_service()
//{
//    return factory.obtain_service<registration_pool_service_i>();
//}

// reward_service_i data_service_factory::reward_service()
//{
//    return factory.obtain_service<reward_service_i>();
//}

// reward_fund_service_i data_service_factory::reward_fund_service()
//{
//    return factory.obtain_service<reward_fund_service_i>();
//}

// vesting_delegation_service_i data_service_factory::vesting_delegation_service()
//{
//    return factory.obtain_service<vesting_delegation_service_i>();
//}

// withdraw_vesting_route_service_i data_service_factory::withdraw_vesting_route_service()
//{
//    return factory.obtain_service<withdraw_vesting_route_service_i>();
//}

// witness_service_i data_service_factory::witness_service()
//{
//    return factory.obtain_service<witness_service_i>();
//}

// witness_vote_service_i data_service_factory::witness_vote_service()
//{
//    return factory.obtain_service<witness_vote_service_i>();
//}

//} // namespace chain
//} // namespace scorum

// clang-format off
DATA_SERVICE_FACTORY_IMPL(
        (account)
        (atomicswap)
        (budget)
        (comment)
        (comment_vote)
        (decline_voting_rights_request)
        (dynamic_global_property)
        (escrow)
        (proposal)
        (registration_committee)
        (registration_pool)
        (reward)
        (reward_fund)
        (vesting_delegation)
        (withdraw_vesting_route)
        (witness)
        (witness_vote)
        )
// clang-format on
