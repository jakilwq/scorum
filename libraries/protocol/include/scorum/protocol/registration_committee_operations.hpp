#pragma once

#include <scorum/protocol/proposal_operations.hpp>
#include <scorum/protocol/committee.hpp>

namespace scorum {
namespace protocol {
namespace registration_committee {

using reg_committee = registration_committee_i;

using invite_operation = proposal::invite_operation<reg_committee>;
using dropout_operation = proposal::dropout_operation<reg_committee>;
using change_add_member_quorum_operation = proposal::change_add_member_quorum_operation<reg_committee>;
using change_exclude_member_quorum_operation = proposal::change_exclude_member_quorum_operation<reg_committee>;
using change_quorum_operation = proposal::change_quorum_operation<reg_committee>;

} // registration_committee
} // namespace chain
} // namespace scorum

FC_REFLECT(scorum::protocol::registration_committee::invite_operation, (new_member))
FC_REFLECT(scorum::protocol::registration_committee::dropout_operation, (member))
FC_REFLECT(scorum::protocol::registration_committee::change_add_member_quorum_operation, (quorum))
FC_REFLECT(scorum::protocol::registration_committee::change_exclude_member_quorum_operation, (quorum))
FC_REFLECT(scorum::protocol::registration_committee::change_quorum_operation, (quorum))
