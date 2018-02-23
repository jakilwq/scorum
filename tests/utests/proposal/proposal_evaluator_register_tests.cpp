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

struct dev_committee_service_i : public committee_service_i
{
    virtual void transfer() = 0;
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

struct dev_committee_service : public dev_committee_service_i
{
    virtual void transfer() override
    {
    }

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
    typedef proposal_operation<CommitteeType> base_operation_type;
};

template <typename CommitteeType> struct proposal_invite_operation : public proposal_operation<CommitteeType>
{
};

template <typename CommitteeType> struct proposal_transfer_operation
{
    typedef CommitteeType committee_type;
    typedef proposal_transfer_operation<CommitteeType> base_operation_type;
};

using reg_commitee_invite_member_operation = proposal_invite_operation<reg_committee_service>;
using dev_commitee_invite_member_operation = proposal_invite_operation<dev_committee_service>;
using dev_commitee_transfer_operation = proposal_transfer_operation<dev_committee_service>;

// clang-format off
using proposal_operations = fc::static_variant<reg_commitee_invite_member_operation,
                                               dev_commitee_invite_member_operation,
                                               dev_commitee_transfer_operation>;
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

    template <typename C> C& obtain()
    {
        FC_THROW_EXCEPTION(fc::assert_exception, "Operation not implemented.");
    }

    template <typename C, typename O> C& obtain(const O&)
    {
        FC_THROW_EXCEPTION(fc::assert_exception, "Operation not implemented.");
    }

    committee_service_i* _reg_committee_service;
    dev_committee_service_i* _dev_committee_service;
};

template <> committee_service_i& committee_factory::obtain_committee(const proposal_operation<reg_committee_service>&)
{
    return *_reg_committee_service;
}

template <> committee_service_i& committee_factory::obtain_committee(const proposal_operation<dev_committee_service>&)
{
    return *_dev_committee_service;
}

template <> dev_committee_service_i& committee_factory::obtain()
{
    return *_dev_committee_service;
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

template <typename OperationType>
struct proposal_transfer_evaluator : public scorum::chain::evaluator_impl<committee_factory,
                                                                          proposal_transfer_evaluator<OperationType>,
                                                                          proposal_operations>
{
    typedef OperationType operation_type;

    proposal_transfer_evaluator(committee_factory& r)
        : scorum::chain::
              evaluator_impl<committee_factory, proposal_transfer_evaluator<OperationType>, proposal_operations>(r)
    {
    }

    void do_apply(const operation_type&)
    {
        auto& committee = this->db().template obtain<dev_committee_service_i>();

        committee.transfer();
    }
};

struct fixture
{
    MockRepository mocks;

    committee_service_i* reg_committee = mocks.Mock<committee_service_i>();
    dev_committee_service_i* dev_committee = mocks.Mock<dev_committee_service_i>();

    committee_factory factory;

    std::vector<std::string> log;

    fixture()
    {
        factory._dev_committee_service = dev_committee;
        factory._reg_committee_service = reg_committee;
    }
};

BOOST_FIXTURE_TEST_CASE(evaluator_register_test, fixture)
{
    std::vector<std::string> log;

    scorum::chain::evaluator_registry<proposal_operations, committee_factory> reg(factory);

    reg.register_evaluator<proposal_invite_evaluator<reg_commitee_invite_member_operation>>();
    reg.register_evaluator<proposal_invite_evaluator<dev_commitee_invite_member_operation>>();
    reg.register_evaluator<proposal_transfer_evaluator<dev_commitee_transfer_operation>>();

    reg_commitee_invite_member_operation op1;
    dev_commitee_invite_member_operation op2;
    dev_commitee_transfer_operation op3;

    auto& evaluator1 = reg.get_evaluator(op1);
    auto& evaluator2 = reg.get_evaluator(op2);
    auto& evaluator3 = reg.get_evaluator(op3);

    mocks.ExpectCall(reg_committee, committee_service_i::invite_member);
    mocks.ExpectCall(dev_committee, dev_committee_service_i::invite_member);
    mocks.ExpectCall(dev_committee, dev_committee_service_i::transfer);

    evaluator1.apply(op1);
    evaluator2.apply(op2);
    evaluator3.apply(op3);
}
