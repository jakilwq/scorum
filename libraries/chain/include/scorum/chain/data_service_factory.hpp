#pragma once

// namespace scorum {
// namespace chain {

// class database;
// class dbservice_dbs_factory;

// struct account_service_i;
// struct atomicswap_service_i;
// struct budget_service_i;
// struct comment_service_i;
// struct comment_vote_service_i;
// struct decline_voting_rights_request_service_i;
// struct dynamic_global_property_service_i;
// struct escrow_service_i;
// struct proposal_service_i;
// struct registration_committee_service_i;
// struct registration_pool_service_i;
// struct reward_service_i;
// struct reward_fund_service_i;
// struct vesting_delegation_service_i;
// struct withdraw_vesting_route_service_i;
// struct witness_service_i;
// struct witness_vote_service_i;

// struct data_service_factory_i
//{
//    virtual account_service_i account_service() = 0;
//    virtual atomicswap_service_i atomicswap_service() = 0;
//    virtual budget_service_i budget_service() = 0;
//    virtual comment_service_i comment_service() = 0;
//    virtual comment_vote_service_i comment_vote_service() = 0;
//    virtual decline_voting_rights_request_service_i decline_voting_rights_request_service() = 0;
//    virtual dynamic_global_property_service_i dynamic_global_property_service() = 0;
//    virtual escrow_service_i escrow_service() = 0;
//    virtual proposal_service_i proposal_service() = 0;
//    virtual registration_committee_service_i registration_committee_service() = 0;
//    virtual registration_pool_service_i registration_pool_service() = 0;
//    virtual reward_service_i reward_service() = 0;
//    virtual reward_fund_service_i reward_fund_service() = 0;
//    virtual vesting_delegation_service_i vesting_delegation_service() = 0;
//    virtual withdraw_vesting_route_service_i withdraw_vesting_route_service() = 0;
//    virtual witness_service_i witness_service() = 0;
//    virtual witness_vote_service_i witness_vote_service() = 0;
//};

// struct data_service_factory : public data_service_factory_i
//{
//    explicit data_service_factory(scorum::chain::database& db);
//    virtual ~data_service_factory();

//    virtual account_service_i account_service() override;
//    virtual atomicswap_service_i atomicswap_service() override;
//    virtual budget_service_i budget_service() override;
//    virtual comment_service_i comment_service() override;
//    virtual comment_vote_service_i comment_vote_service() override;
//    virtual decline_voting_rights_request_service_i decline_voting_rights_request_service() override;
//    virtual dynamic_global_property_service_i dynamic_global_property_service() override;
//    virtual escrow_service_i escrow_service() override;
//    virtual proposal_service_i proposal_service() override;
//    virtual registration_committee_service_i registration_committee_service() override;
//    virtual registration_pool_service_i registration_pool_service() override;
//    virtual reward_service_i reward_service() override;
//    virtual reward_fund_service_i reward_fund_service() override;
//    virtual vesting_delegation_service_i vesting_delegation_service() override;
//    virtual withdraw_vesting_route_service_i withdraw_vesting_route_service() override;
//    virtual witness_service_i witness_service() override;
//    virtual witness_vote_service_i witness_vote_service() override;

// private:
//    dbservice_dbs_factory& factory;
//};

//} // namespace chain
//} // namespace scorum

// account
// atomicswap
// budget
// comment
// comment_vote
// decline_voting_rights_request
// dynamic_global_property
// escrow
// proposal
// registration_committee
// registration_pool
// reward
// reward_fund
// vesting_delegation
// withdraw_vesting_route
// witness
// witness_vote

#include <scorum/chain/data_service_factory_def.hpp>

// clang-format off
DATA_SERVICE_FACTORY_DECLARE(
        (account)
        (atomicswap)
        (budget)
        (comment)
        (comment_vote)
        (decline_voting_rights_request)
        (dynamic_global_property)
        (escrow)
        (proposal)
        (registration_committee)
        (registration_pool)
        (reward)
        (reward_fund)
        (vesting_delegation)
        (withdraw_vesting_route)
        (witness)
        (witness_vote)
        )
// clang-format on
