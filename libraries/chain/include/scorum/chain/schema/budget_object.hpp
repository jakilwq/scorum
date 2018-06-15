#pragma once

#include <fc/fixed_string.hpp>

#include <scorum/protocol/authority.hpp>
#include <scorum/protocol/scorum_operations.hpp>

#include <scorum/chain/schema/scorum_object_types.hpp>
#include <scorum/chain/schema/witness_objects.hpp>

#include <boost/multi_index/composite_key.hpp>

#include <numeric>

namespace scorum {
namespace chain {

using scorum::protocol::asset;

class budget_object : public object<budget_object_type, budget_object>
{
public:
    CHAINBASE_DEFAULT_DYNAMIC_CONSTRUCTOR(budget_object, (content_permlink))

    id_type id;

    budget_type type = budget_type::internal;

    account_name_type owner;
    fc::shared_string content_permlink;

    time_point_sec created = time_point_sec::min();
    time_point_sec deadline = time_point_sec::maximum();

    // in balance stored both SCORUM_SYMBOL and SP_SYMBOL tockens
    share_type balance;
    share_type per_block;

    uint32_t last_cashout_block = 0;
};

struct by_owner_name;
struct by_uniq_meta_data;
struct by_per_block;

typedef shared_multi_index_container<budget_object,
                                     indexed_by<ordered_unique<tag<by_id>,
                                                               member<budget_object,
                                                                      budget_id_type,
                                                                      &budget_object::id>>,
                                                ordered_non_unique<tag<by_owner_name>,
                                                                   member<budget_object,
                                                                          account_name_type,
                                                                          &budget_object::owner>>,
                                                ordered_unique<tag<by_uniq_meta_data>,
                                                               composite_key<budget_object,
                                                                             member<budget_object,
                                                                                    account_name_type,
                                                                                    &budget_object::owner>,
                                                                             member<budget_object,
                                                                                    fc::shared_string,
                                                                                    &budget_object::content_permlink>>>,
                                                ordered_unique<tag<by_per_block>,
                                                               composite_key<budget_object,
                                                                             member<budget_object,
                                                                                    budget_type,
                                                                                    &budget_object::type>,
                                                                             member<budget_object,
                                                                                    share_type,
                                                                                    &budget_object::per_block>,
                                                                             member<budget_object,
                                                                                    budget_id_type,
                                                                                    &budget_object::id>>,
                                                               composite_key_compare<std::less<budget_type>,
                                                                                     std::greater<share_type>,
                                                                                     std::less<budget_id_type>>>>>
    budget_index;
} // namespace chain
} // namespace scorum

FC_REFLECT(scorum::chain::budget_object,
           (id)(type)(owner)(content_permlink)(created)(deadline)(balance)(per_block)(last_cashout_block))

CHAINBASE_SET_INDEX_TYPE(scorum::chain::budget_object, scorum::chain::budget_index)
