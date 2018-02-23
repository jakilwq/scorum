#include <boost/test/unit_test.hpp>

#include <fc/io/json.hpp>

#include <scorum/chain/evaluators/proposal_evaluator.hpp>
#include <scorum/chain/services/registration_committee.hpp>

#include <scorum/protocol/registration_committee_operations.hpp>

#include <hippomocks.h>

namespace xxx {

using namespace scorum::chain;
using namespace scorum::protocol;

BOOST_AUTO_TEST_SUITE(proposal_evaluator_tests)

struct fixture
{
    MockRepository mocks;

    data_service_factory_i* services = mocks.Mock<data_service_factory_i>();
    registration_committee_service_i* committee_service = mocks.Mock<registration_committee_service_i>();

    fixture()
    {
        mocks.ExpectCall(services, data_service_factory_i::registration_committee_service)
            .ReturnByRef(*committee_service);
    }

    template <typename OperationType, typename CommitteeServiceType>
    void add_member_in_committee()
    {
        mocks.ExpectCall(committee_service, CommitteeServiceType::add_member);

        OperationType op;
        op.new_member = "alice";

        committee_factory factory(*services);

        proposal_invite_evaluator<OperationType> e(factory);

        e.do_apply(op);
    }
};

BOOST_FIXTURE_TEST_CASE(add_member_in_committee_test, fixture)
{
    add_member_in_committee<registration_committee::invite_operation, registration_committee_service_i>();
    //    add_member_in_committee<dev_commitee_invite_member_operation, dev_committee_service_i>();
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(proposal_operation_serialization_tests)

BOOST_AUTO_TEST_CASE(serialize_proposal_invite_operation)
{
    registration_committee::invite_operation op;
    op.new_member = "alice";

    BOOST_CHECK_EQUAL(R"({"new_member":"alice"})", fc::json::to_string(op));
}

} // namespace xxx

// BOOST_AUTO_TEST_CASE(serialize_proposal_vote_operation)
//{
//    registration_committee::invite_operation invite_operation;
//    invite_operation.new_member = "bob";

//    proposal_create_operation2 create_operation;

//    create_operation.creator = "alice";
//    create_operation.operation = invite_operation;

//    BOOST_CHECK_EQUAL(R"({"creator":"alice","operation":[0,{"new_member":"bob"}]})",
//                      fc::json::to_string(create_operation));
//}

BOOST_AUTO_TEST_SUITE_END()

namespace testing {

namespace proposal {

struct reg
{
};
struct dev
{
};

template <typename T>
struct add_member_operation
{
    scorum::protocol::account_name_type account;
};

template <typename T>
struct exclude_member_operation
{
    scorum::protocol::account_name_type account;
};

using operation = fc::static_variant<add_member_operation<reg>, exclude_member_operation<reg>>;

} // namespace proposal

struct create_proposal_operation
{
    proposal::operation operation;
};

using operations = fc::static_variant<create_proposal_operation>;

struct transaction
{
    operations operation;
};

BOOST_AUTO_TEST_CASE(xxx)
{
    proposal::add_member_operation<proposal::reg> add_member_op;
    add_member_op.account = "alice";

    create_proposal_operation op;
    op.operation = add_member_op;

    transaction trx;
    trx.operation = op;

    BOOST_CHECK_EQUAL(R"({"operation":["create_proposal",{"operation":["add_member",{"account":"alice"}]}]})",
                      fc::json::to_string(trx));
}

} // namespace testing

FC_REFLECT_TEMPLATE((typename reg), testing::proposal::add_member_operation<reg>, (account))
FC_REFLECT_TEMPLATE((typename T), testing::proposal::exclude_member_operation<T>, (account))
// FC_REFLECT(testing::proposal::add_member_operation<testing::proposal::reg>, (account))
// FC_REFLECT(testing::proposal::exclude_member_operation<testing::proposal::reg>, (account))
FC_REFLECT_TYPENAME(testing::proposal::operation)

FC_REFLECT(testing::create_proposal_operation, (operation))
FC_REFLECT_TYPENAME(testing::operations)

FC_REFLECT(testing::transaction, (operation))

#include <scorum/protocol/operation_util_impl.hpp>

DECLARE_OPERATION_X_TYPE(testing::proposal::operation)
DEFINE_OPERATION_X_TYPE(testing::proposal::operation)

DECLARE_OPERATION_X_TYPE(testing::operations)
DEFINE_OPERATION_X_TYPE(testing::operations)

struct trx
{
    scorum::protocol::operation op;
};

BOOST_AUTO_TEST_CASE(two)
{
    scorum::protocol::proposal_create_operation o;
    o.creator = "alice";

    trx t;
    t.op = o;

    BOOST_CHECK_EQUAL("", fc::json::to_string(t));
}

FC_REFLECT(trx, (op))
