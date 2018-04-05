#pragma once

#include <scorum/protocol/scorum_operations.hpp>
#include <scorum/chain/evaluators/evaluator.hpp>

#include <scorum/protocol/types.hpp>

namespace scorum {
namespace chain {

class data_service_factory_i;
class account_service_i;
class dynamic_global_property_service_i;
class comment_service_i;

class comment_evaluator : public evaluator_impl<data_service_factory_i, comment_evaluator>
{
public:
    using operation_type = scorum::protocol::comment_operation;

    comment_evaluator(data_service_factory_i& services);

    void do_apply(const operation_type& op);

private:
    account_service_i& _account_service;
    comment_service_i& _comment_service;
    dynamic_global_property_service_i& _properties_service;
};

} // namespace chain
} // namespace scorum
