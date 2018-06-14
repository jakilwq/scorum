#include <boost/test/unit_test.hpp>

#include <scorum/chain/data_service_factory.hpp>
#include <scorum/chain/services/dev_pool.hpp>

#include <scorum/chain/evaluators/proposal_evaluators.hpp>

#include "defines.hpp"

#include "object_wrapper.hpp"

#include <hippomocks.h>

namespace development_committee_top_budgets_evaluator_tests {

using namespace scorum::chain;
using namespace scorum::protocol;

SCORUM_TEST_CASE(validate_development_committee_top_budgets_operaton)
{
    development_committee_change_top_budgets_amount_operation::operation_type op;

    SCORUM_REQUIRE_THROW(op.validate(), fc::assert_exception);

    op.amount = 0;

    SCORUM_REQUIRE_THROW(op.validate(), fc::assert_exception);

    op.amount = SCORUM_DEFAULT_TOP_BUDGETS_AMOUNT;

    BOOST_CHECK_NO_THROW(op.validate());

    op.amount = 111;

    BOOST_CHECK_NO_THROW(op.validate());
}

struct fixture : public shared_memory_fixture
{
    MockRepository mocks;

    data_service_factory_i* services = mocks.Mock<data_service_factory_i>();

    dev_pool_service_i* dev_pool_service = mocks.Mock<dev_pool_service_i>();

    fixture()
        : shared_memory_fixture()
    {
        mocks.ExpectCall(services, data_service_factory_i::dev_pool_service).ReturnByRef(*dev_pool_service);
    }
};

BOOST_FIXTURE_TEST_CASE(change_top_budgets_amount, fixture)
{
    development_committee_change_top_budgets_amount_operation::operation_type op;

    op.amount = 111;

    development_committee_change_top_budgets_amount_evaluator evaluator(*services);

    dev_committee_object dev_committee = create_object<dev_committee_object>(shm);

    BOOST_CHECK_NE(dev_committee.top_budgets_amount, op.amount);

    mocks
        .ExpectCallOverload(dev_pool_service,
                            (void (dev_pool_service_i::*)(const dev_pool_service_i::modifier_type&))
                                & dev_pool_service_i::update)
        .Do([&](const dev_pool_service_i::modifier_type& m) { m(dev_committee); });

    BOOST_CHECK_NO_THROW(evaluator.do_apply(op));

    BOOST_CHECK_EQUAL(dev_committee.top_budgets_amount, op.amount);
}

} // namespace development_committee_top_budgets_evaluator_tests
