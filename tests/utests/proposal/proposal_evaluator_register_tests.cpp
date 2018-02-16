#include <boost/test/unit_test.hpp>

#include <scorum/chain/evaluators/evaluator.hpp>
#include <scorum/chain/evaluators/evaluator_registry.hpp>

#include <scorum/chain/data_service_factory.hpp>

#include <fc/static_variant.hpp>

#include <functional>
#include <hippomocks.h>

struct committee_service_i
{
    virtual void invite_member(const std::string& name) = 0;
    virtual void drop_member(const std::string& name) = 0;
    virtual void change_quorum() = 0;
};

struct reg_committee_service : public committee_service_i
{
    virtual void invite_member(const std::string& name) override
    {
    }

    virtual void drop_member(const std::string& name) override
    {
    }

    virtual void change_quorum() override
    {
    }
};

struct dev_committee_service : public committee_service_i
{
    virtual void invite_member(const std::string& name) override
    {
    }

    virtual void drop_member(const std::string& name) override
    {
    }

    virtual void change_quorum() override
    {
    }
};

template <typename CommitteeType> struct proposal_operation
{
    typedef CommitteeType committee_type;
};

template <typename BaseOperation> struct proposal_invite_operation : public BaseOperation
{
    typedef BaseOperation base_operation_type;
};

using reg_commitee_invite_member_operation = proposal_invite_operation<proposal_operation<reg_committee_service>>;
using dev_commitee_invite_member_operation = proposal_invite_operation<proposal_operation<dev_committee_service>>;

// clang-format off
using proposal_operations = fc::static_variant<reg_commitee_invite_member_operation,
                                               dev_commitee_invite_member_operation>;
// clang-format on

struct committee_factory
{
    committee_factory()
    {
    }

    template <typename OperationType> committee_service_i& obtain_committee(const OperationType&)
    {
        FC_THROW_EXCEPTION(fc::assert_exception, "Operation not implemented.");
    }

    enum committee_type
    {
        reg,
        dev
    };

    fc::flat_map<committee_type, committee_service_i*> storage;
};

template <> committee_service_i& committee_factory::obtain_committee(const proposal_operation<reg_committee_service>&)
{
    return *storage[committee_type::reg];
}

template <> committee_service_i& committee_factory::obtain_committee(const proposal_operation<dev_committee_service>&)
{
    return *storage[committee_type::dev];
}

template <typename OperationType>
struct proposal_invite_evaluator : public scorum::chain::evaluator_impl<committee_factory,
                                                                        proposal_invite_evaluator<OperationType>,
                                                                        proposal_operations>
{
    typedef OperationType operation_type;

    proposal_invite_evaluator(committee_factory& r)
        : scorum::chain::
              evaluator_impl<committee_factory, proposal_invite_evaluator<OperationType>, proposal_operations>(r)
    {
    }

    void do_apply(const operation_type& o)
    {
        const typename operation_type::base_operation_type& bo = o;
        auto& committee = this->db().obtain_committee(bo);

        committee.invite_member("alice");
    }
};

struct fixture
{
    MockRepository mocks;

    committee_service_i* reg_committee = mocks.Mock<committee_service_i>();
    committee_service_i* dev_committee = mocks.Mock<committee_service_i>();

    committee_factory factory;

    std::vector<std::string> log;

    fixture()
    {
        factory.storage[committee_factory::dev] = dev_committee;
        factory.storage[committee_factory::reg] = reg_committee;
    }
};

BOOST_FIXTURE_TEST_CASE(evaluator_register_test, fixture)
{
    std::vector<std::string> log;

    scorum::chain::evaluator_registry<proposal_operations, committee_factory> reg(factory);

    reg.register_evaluator<proposal_invite_evaluator<reg_commitee_invite_member_operation>>();
    reg.register_evaluator<proposal_invite_evaluator<dev_commitee_invite_member_operation>>();

    reg_commitee_invite_member_operation op1;
    dev_commitee_invite_member_operation op2;

    auto& evaluator1 = reg.get_evaluator(op1);
    auto& evaluator2 = reg.get_evaluator(op2);

    mocks.ExpectCall(reg_committee, committee_service_i::invite_member);
    mocks.ExpectCall(dev_committee, committee_service_i::invite_member);

    evaluator1.apply(op1);
    evaluator2.apply(op2);
}
