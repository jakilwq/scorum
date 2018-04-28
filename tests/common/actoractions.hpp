#pragma once

#include "actor.hpp"

#include <scorum/protocol/asset.hpp>

using scorum::protocol::share_value_type;

namespace database_fixture {
class database_trx_integration_fixture;
}

class ActorActions
{
public:
    using chain_type = database_fixture::database_trx_integration_fixture;

    ActorActions(chain_type& chain, const Actor& a);

    void create_account(const Actor& a);
    void transfer_to_scorumpower(const Actor& a, asset amount);
    void transfer(const Actor& a, asset amount);

    void give_scr(const Actor& a, share_value_type amount);
    void give_sp(const Actor& a, share_value_type amount);

private:
    chain_type& _chain;
    const Actor& _actor;
};
