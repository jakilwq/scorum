#pragma once

#include <scorum/protocol/types.hpp>

namespace scorum {
namespace chain {

struct committee_service_i
{
    virtual void add_member(const protocol::account_name_type&) = 0;
    virtual void exclude_member(const protocol::account_name_type&) = 0;
    //    virtual void change_invite_quorum() = 0;
    //    virtual void change_dropout_quorum() = 0;
    //    virtual void change_quorum() = 0;
};

} // namespace chain
} // namespace scorum
