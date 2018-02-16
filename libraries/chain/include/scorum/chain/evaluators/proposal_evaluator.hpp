#pragma once

#include <scorum/chain/committee_factory.hpp>
#include <scorum/chain/evaluators/evaluator.hpp>

#include <scorum/chain/evaluators/proposal_operations.hpp>

namespace scorum {
namespace chain {

template <typename OperationType>
struct proposal_evaluator_impl
    : public evaluator_impl<committee_factory, proposal_evaluator_impl<OperationType>, proposal_operations>
{
    typedef OperationType operation_type;

    proposal_evaluator_impl(committee_factory& factory)
        : evaluator_impl<committee_factory, proposal_evaluator_impl<OperationType>, proposal_operations>(factory)
    {
    }

    virtual void do_apply(const OperationType&) = 0;
};

template <typename OperationType> struct proposal_invite_evaluator : public proposal_evaluator_impl<OperationType>
{
    typedef OperationType operation_type;

    proposal_invite_evaluator(committee_factory& factory)
        : proposal_evaluator_impl<OperationType>(factory)
    {
    }

    void do_apply(const OperationType& o) override
    {
        const typename OperationType::base_operation_type& bo = o;
        auto& committee = this->db().obtain_committee(bo);

        committee.add_member(o.new_member);
    }
};

} // namespace chain
} // namespace scorum
