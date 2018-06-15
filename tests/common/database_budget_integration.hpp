#pragma once

#include <scorum/protocol/operations.hpp>
#include "database_trx_integration.hpp"
#include <functional>

namespace database_fixture {

struct database_budget_integration_fixture : public database_trx_integration_fixture
{
    database_budget_integration_fixture();

    class advertising_budget_op
    {
    public:
        advertising_budget_op(database_budget_integration_fixture& fixture,
                              const create_budget_operation& op,
                              fc::ecc::private_key actor_private_key);
        advertising_budget_op& operator=(const advertising_budget_op&);

        advertising_budget_op& push();
        advertising_budget_op& push_in_block();

        std::string owner() const
        {
            return my.owner;
        }
        std::string permlink() const
        {
            return my.content_permlink;
        }

    private:
        database_budget_integration_fixture& fixture;
        fc::ecc::private_key actor_private_key;
        create_budget_operation my;
        bool is_pushed = false;
    };

    advertising_budget_op create_advertising_budget(const budget_type type,
                                                    Actor& actor,
                                                    const asset& balance,
                                                    const fc::time_point_sec& deadline);

private:
    std::string get_unique_permlink();
};
}
