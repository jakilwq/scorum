#pragma once

#include <scorum/protocol/types.hpp>

namespace scorum {
namespace protocol {

template <typename CommitteeType>
struct proposal_operation
{
    typedef CommitteeType committee_type;
    typedef proposal_operation<CommitteeType> base_operation_type;
};

namespace proposal {

template <typename CommitteeType>
struct invite_operation : public proposal_operation<CommitteeType>
{
    account_name_type new_member;
};

template <typename CommitteeType>
struct dropout_operation : public proposal_operation<CommitteeType>
{
    account_name_type member;
};

template <typename CommitteeType>
struct base_change_quorum_operation : public proposal_operation<CommitteeType>
{
    uint16_t quorum;
};

template <typename CommitteeType>
struct change_add_member_quorum_operation : public base_change_quorum_operation<CommitteeType>
{
};

template <typename CommitteeType>
struct change_exclude_member_quorum_operation : public base_change_quorum_operation<CommitteeType>
{
};

template <typename CommitteeType>
struct change_quorum_operation : public base_change_quorum_operation<CommitteeType>
{
};

} // namespace proposal
} // namespace protocol
} // namespace scorum
