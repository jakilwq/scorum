#pragma once

#include <scorum/chain/schema/account_objects.hpp>
#include <scorum/chain/schema/atomicswap_objects.hpp>
#include <scorum/chain/schema/block_summary_object.hpp>
#include <scorum/chain/schema/budget_object.hpp>
#include <scorum/chain/schema/chain_property_object.hpp>
#include <scorum/chain/schema/comment_objects.hpp>
#include <scorum/chain/schema/committee.hpp>
#include <scorum/chain/schema/dev_committee_object.hpp>
#include <scorum/chain/schema/dynamic_global_property_object.hpp>
#include <scorum/chain/schema/proposal_object.hpp>
#include <scorum/chain/schema/registration_objects.hpp>
#include <scorum/chain/schema/reward_balancer_objects.hpp>
#include <scorum/chain/schema/reward_objects.hpp>
#include <scorum/chain/schema/scorum_objects.hpp>
#include <scorum/chain/schema/shared_authority.hpp>
#include <scorum/chain/schema/transaction_object.hpp>
#include <scorum/chain/schema/withdraw_scorumpower_objects.hpp>
#include <scorum/chain/schema/witness_objects.hpp>
#include <scorum/chain/hardfork.hpp>

#include <scorum/typeid/extraction.hpp>

namespace fc {

using scorum::protocol::account_name_type;

using account_name_flat_set_type = fc::shared_flat_set<account_name_type>;

template <typename Stream> Stream& operator<<(Stream& stream, const account_name_flat_set_type& fs)
{
    size_t sz = fs.size();
    stream << sz;
    for (size_t ci = 0; ci < sz; ++ci)
    {
        stream << (*fs.nth(ci));
    }
    return stream;
}

template <typename Stream> Stream& operator>>(Stream& stream, account_name_flat_set_type& fs)
{
    size_t sz = 0;
    stream >> sz;
    for (size_t ci = 0; ci < sz; ++ci)
    {
        account_name_type a;
        stream >> a;
        fs.insert(a);
    }
    return stream;
}
}
using namespace scorum::chain;

// clang-format off
//
// Required exact object type declarations (in schema folders)!
// For missed types we receive error: invalid application of ‘sizeof’ to incomplete type ‘scorum::get_object_type<?>’’
//
SCORUM_OBJECT_TYPE_EXTRACTION(object_type,
                              (account_authority)
                              (account)
                              (account_blogging_statistic)
                              (account_recovery_request)
                              (atomicswap_contract)
                              (block_summary)
                              (budget)
                              (chain_property)
                              (change_recovery_account_request)
                              (account_registration_bonus)
                              (comment)
                              (comment_statistic_scr)
                              (comment_statistic_sp)
                              (comment_vote)
                              (decline_voting_rights_request)
                              (dynamic_global_property)
                              (escrow)
                              (hardfork_property)
                              (owner_authority_history)
                              (proposal)
                              (registration_committee_member)
                              (registration_pool)
                              (content_reward_fund_scr)
                              (content_reward_fund_sp)
                              (content_fifa_world_cup_2018_bounty_reward_fund)
                              (content_reward_balancer_scr)
                              (voters_reward_balancer_scr)
                              (voters_reward_balancer_sp)
                              (transaction)
                              (scorumpower_delegation_expiration)
                              (scorumpower_delegation)
                              (withdraw_scorumpower_route)
                              (withdraw_scorumpower_route_statistic)
                              (withdraw_scorumpower)
                              (witness)
                              (witness_schedule)
                              (witness_vote)
                              (dev_committee)
                              (dev_committee_member)
                              (witness_reward_in_sp_migration)
                              )

//TODO: move to SCORUM_OBJECT_TYPE_EXTRACTION
FC_REFLECT(account_authority_object::id_type, (_id))
FC_REFLECT(account_object::id_type, (_id))
FC_REFLECT(account_blogging_statistic_object::id_type, (_id))
FC_REFLECT(account_recovery_request_object::id_type, (_id))
FC_REFLECT(atomicswap_contract_object::id_type, (_id))
FC_REFLECT(block_summary_object::id_type, (_id))
FC_REFLECT(budget_object::id_type, (_id))
FC_REFLECT(chain_property_object::id_type, (_id))
FC_REFLECT(change_recovery_account_request_object::id_type, (_id))
FC_REFLECT(account_registration_bonus_object::id_type, (_id))
FC_REFLECT(comment_object::id_type, (_id))
FC_REFLECT(comment_statistic_scr_object::id_type, (_id))
FC_REFLECT(comment_statistic_sp_object::id_type, (_id))
FC_REFLECT(comment_vote_object::id_type, (_id))
FC_REFLECT(decline_voting_rights_request_object::id_type, (_id))
FC_REFLECT(dynamic_global_property_object::id_type, (_id))
FC_REFLECT(escrow_object::id_type, (_id))
FC_REFLECT(hardfork_property_object::id_type, (_id))
FC_REFLECT(owner_authority_history_object::id_type, (_id))
FC_REFLECT(proposal_object::id_type, (_id))
FC_REFLECT(registration_committee_member_object::id_type, (_id))
FC_REFLECT(registration_pool_object::id_type, (_id))
FC_REFLECT(content_reward_fund_scr_object::id_type, (_id))
FC_REFLECT(content_reward_fund_sp_object::id_type, (_id))
FC_REFLECT(content_fifa_world_cup_2018_bounty_reward_fund_object::id_type, (_id))
FC_REFLECT(content_reward_balancer_scr_object::id_type, (_id))
FC_REFLECT(voters_reward_balancer_scr_object::id_type, (_id))
FC_REFLECT(voters_reward_balancer_sp_object::id_type, (_id))
FC_REFLECT(transaction_object::id_type, (_id))
FC_REFLECT(scorumpower_delegation_expiration_object::id_type, (_id))
FC_REFLECT(scorumpower_delegation_object::id_type, (_id))
FC_REFLECT(withdraw_scorumpower_route_object::id_type, (_id))
FC_REFLECT(withdraw_scorumpower_route_statistic_object::id_type, (_id))
FC_REFLECT(withdraw_scorumpower_object::id_type, (_id))
FC_REFLECT(witness_object::id_type, (_id))
FC_REFLECT(witness_schedule_object::id_type, (_id))
FC_REFLECT(witness_vote_object::id_type, (_id))
FC_REFLECT(dev_committee_object::id_type, (_id))
FC_REFLECT(dev_committee_member_object::id_type, (_id))
FC_REFLECT(witness_reward_in_sp_migration_object::id_type, (_id))

// clang-format on
