#pragma once

#include <fc/static_variant.hpp>
#include <scorum/chain/services/registration_committee.hpp>

namespace scorum {
namespace chain {

using reg_committee_service = registration_committee_service_i;

template <typename CommitteeType> struct proposal_operation
{
    typedef CommitteeType committee_type;
};

template <typename BaseOperation> struct proposal_invite_operation : public BaseOperation
{
    typedef BaseOperation base_operation_type;

    account_name_type new_member;
};

using reg_commitee_invite_member_operation = proposal_invite_operation<proposal_operation<reg_committee_service>>;
// using dev_commitee_invite_member_operation = proposal_invite_operation<proposal_operation<dev_committee_service>>;

// clang-format off
using proposal_operations = fc::static_variant<
                                                reg_commitee_invite_member_operation
//                                              , dev_commitee_invite_member_operation
                                              >;
// clang-format on

struct proposal_vote_operation2
{
    account_name_type creator;
    proposal_operations operation;
};

} // namespace chain
} // namespace scorum

FC_REFLECT(
    scorum::chain::proposal_invite_operation<scorum::chain::proposal_operation<scorum::chain::reg_committee_service>>,
    (new_member))

FC_REFLECT(scorum::chain::proposal_vote_operation2, (creator)(operation))
