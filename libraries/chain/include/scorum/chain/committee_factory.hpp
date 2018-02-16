#pragma once 

#include <scorum/chain/data_service_factory.hpp>
#include <scorum/chain/evaluators/proposal_operations.hpp>
#include <fc/exception/exception.hpp>

namespace scorum {
namespace chain {

struct committee_factory
{
    committee_factory(data_service_factory_i& factory)
        : _factory(factory)
    {
    }

    template <typename OperationType> committee_service_i& obtain_committee(const OperationType& o)
    {
        FC_THROW_EXCEPTION(fc::assert_exception, "Operation not implemented.");
    }

    scorum::chain::data_service_factory_i& _factory;
};

template <>
committee_service_i& committee_factory::obtain_committee(const proposal_operation<registration_committee_service_i>&)
{
    return _factory.registration_committee_service();
}

} // namespace chain
} // namespace scorum
