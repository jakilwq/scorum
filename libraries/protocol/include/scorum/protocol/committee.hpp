#pragma once

#include <scorum/protocol/types.hpp>

namespace scorum {
namespace protocol {

struct committee_service_i
{
    virtual void add_member(const account_name_type&) = 0;
    virtual void exclude_member(const account_name_type&) = 0;
    //    virtual void change_add_member_quorum() = 0;
    //    virtual void change_exclude_member_quorum() = 0;
    //    virtual void change_quorum() = 0;
};

struct registration_committee_i : public committee_service_i
{
};

struct development_committee_i : public committee_service_i
{
    virtual void transfer() = 0;
};

} // namespace protocol
} // namespace scorum
