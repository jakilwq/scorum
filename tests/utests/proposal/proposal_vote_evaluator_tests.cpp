#include <boost/test/unit_test.hpp>

#include <scorum/chain/data_service_factory.hpp>
#include <scorum/chain/evaluators/proposal_vote_evaluator2.hpp>

#include <hippomocks.h>

using namespace scorum::chain;
using namespace scorum::protocol;

struct fixture
{
    MockRepository mocks;

    data_service_factory_i* services = mocks.Mock<data_service_factory_i>();
};

BOOST_FIXTURE_TEST_CASE(test_x, fixture)
{
    proposal_vote_evaluator2 evaluator(*services);
}
