#include "database_budget_integration.hpp"

namespace database_fixture {

database_budget_integration_fixture::database_budget_integration_fixture()
{
}

database_budget_integration_fixture::advertising_budget_op::advertising_budget_op(
    database_budget_integration_fixture& fixture,
    const create_budget_operation& op,
    fc::ecc::private_key actor_private_key)
    : fixture(fixture)
    , actor_private_key(actor_private_key)
    , my(op)
{
}

database_budget_integration_fixture::advertising_budget_op&
database_budget_integration_fixture::advertising_budget_op::push()
{
    is_pushed = true;
    fixture.push_operation(my, actor_private_key, false);
    return *this;
}

database_budget_integration_fixture::advertising_budget_op&
database_budget_integration_fixture::advertising_budget_op::push_in_block()
{
    if (!is_pushed)
        push();

    fixture.generate_block();

    return *this;
}

database_budget_integration_fixture::advertising_budget_op
database_budget_integration_fixture::create_advertising_budget(Actor& actor, asset balance, fc::time_point_sec deadline)
{
    create_budget_operation op;
    op.owner = actor.name;
    op.content_permlink = get_unique_permlink();
    op.balance = balance;
    op.deadline = deadline;

    return advertising_budget_op(*this, op, actor.private_key);
}

std::string database_budget_integration_fixture::get_unique_permlink()
{
    static uint32_t permlink_no = 0;
    permlink_no++;

    return boost::lexical_cast<std::string>(permlink_no);
}
}
