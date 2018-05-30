#pragma once

#include <scorum/snapshot/get_types_by_id.hpp>

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

using namespace scorum::chain;

// clang-format off
//
// Required exact object type declarations (in schema folders)!
// For missed types we receive error: invalid application of ‘sizeof’ to incomplete type ‘scorum::get_object_type<?>’’
//
SCORUM_OBJECT_TYPES_FOR_SNAPSHOT_SECTION(base_section, object_type,
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
// clang-format on
