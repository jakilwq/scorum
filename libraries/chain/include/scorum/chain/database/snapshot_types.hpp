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
                              (transaction)
                              )
// clang-format on
