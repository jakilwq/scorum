#pragma once

#include <fc/static_variant.hpp>

#include <scorum/protocol/authority.hpp>
#include <scorum/protocol/types.hpp>
#include <scorum/protocol/registration_committee_operations.hpp>

namespace scorum {
namespace protocol {

using proposal_operations = fc::static_variant<registration_committee::invite_operation,
                                               registration_committee::dropout_operation,
                                               registration_committee::change_add_member_quorum_operation,
                                               registration_committee::change_exclude_member_quorum_operation,
                                               registration_committee::change_quorum_operation>;

} // namespace chain
} // namespace scorum

// Place DECLARE_OPERATION_TYPE in a .hpp file to declare
// functions related to your operation type
//
// namespace fc {

// void to_variant(const scorum::protocol::proposal_operations&, fc::variant&);
// void from_variant(const fc::variant&, scorum::protocol::proposal_operations&);

//} // namespace fc

// DECLARE_OPERATION_TYPE2(scorum::protocol::proposal_operations)
FC_REFLECT_TYPENAME(scorum::protocol::proposal_operations)
