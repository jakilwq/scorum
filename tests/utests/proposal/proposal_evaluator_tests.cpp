#include <boost/test/unit_test.hpp>

#include <fc/io/json.hpp>

#include <scorum/chain/evaluators/proposal_operations.hpp>
#include <scorum/chain/evaluators/proposal_evaluator.hpp>
#include <scorum/chain/services/registration_committee.hpp>

#include <hippomocks.h>

using namespace scorum::chain;

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

    template <typename OperationType, typename CommitteeServiceType> void add_member_in_committee()
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
    add_member_in_committee<reg_commitee_invite_member_operation, registration_committee_service_i>();
    //    add_member_in_committee<dev_commitee_invite_member_operation, dev_committee_service_i>();
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(proposal_operation_serialization_tests)

BOOST_AUTO_TEST_CASE(serialize_proposal_invite_operation)
{
    reg_commitee_invite_member_operation op;
    op.new_member = "alice";

    BOOST_CHECK_EQUAL(R"({"new_member":"alice"})", fc::json::to_string(op));
}

BOOST_AUTO_TEST_CASE(serialize_proposal_vote_operation)
{
    reg_commitee_invite_member_operation invite_operation;
    invite_operation.new_member = "bob";

    scorum::chain::proposal_vote_operation2 vote_operation;

    vote_operation.creator = "alice";
    vote_operation.operation = invite_operation;

    BOOST_CHECK_EQUAL(R"({"creator":"alice","operation":[0,{"new_member":"bob"}]})",
                      fc::json::to_string(vote_operation));
}

BOOST_AUTO_TEST_SUITE_END()
