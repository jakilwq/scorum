#pragma once

#include <scorum/protocol/types.hpp>
#include <scorum/tags/tags_objects.hpp>
#include <scorum/tags/tags_api_objects.hpp>
#include <scorum/tags/tags_service.hpp>
#include <scorum/tags/tags_plugin.hpp>

#include <scorum/chain/data_service_factory.hpp>
#include <scorum/chain/services/comment.hpp>
#include <scorum/chain/services/reward_fund.hpp>
#include <scorum/chain/services/comment_vote.hpp>
#include <scorum/chain/services/account.hpp>

#include <scorum/chain/util/reward.hpp>

namespace scorum {

namespace chain {
class database;
}

namespace tags {

class tags_api_impl
{
public:
    tags_api_impl(scorum::chain::data_service_factory_i& services, tags_service_i& tags_service)
        : _services(services)
        , _tags_service(tags_service)
    {
    }

    std::vector<api::discussion> dummy() const
    {
        std::vector<api::discussion> result;
        return result;
    }

    std::vector<api::tag_api_obj> get_trending_tags(const std::string& after_tag, uint32_t limit) const
    {
        std::vector<api::tag_api_obj> result;
        return result;
    }

    std::vector<std::pair<std::string, uint32_t>> get_tags_used_by_author(const std::string& author) const
    {
        std::vector<std::pair<std::string, uint32_t>> result;
        return result;
    }

    std::vector<api::discussion> get_discussions_by_payout(const api::discussion_query& query) const
    {
        return dummy();
    }

    std::vector<api::discussion> get_post_discussions_by_payout(const api::discussion_query& query) const
    {
        return dummy();
    }

    std::vector<api::discussion> get_comment_discussions_by_payout(const api::discussion_query& query) const
    {
        return dummy();
    }

    std::vector<api::discussion> get_discussions_by_trending(const api::discussion_query& query) const
    {
        return dummy();
    }

    std::vector<api::discussion> get_discussions_by_created(const api::discussion_query& query) const
    {
        query.validate();
        auto tag = fc::to_lower(query.tag);
        //        auto parent = get_parent(query);

        //            //        const auto& tidx =
        //            my->_db.get_index<tag_index>().indices().get<tags::by_parent_created>();
        //            //        auto tidx_itr = tidx.lower_bound(boost::make_tuple(tag, parent,
        //            fc::time_point_sec::maximum()));

        return dummy();
        //        query.validate();
        //        auto tag = fc::to_lower(query.tag);
        //        auto parent = get_parent(query);

        //        const auto& tidx = my->_db.get_index<tag_index>().indices().get<tags::by_parent_created>();
        //        auto tidx_itr = tidx.lower_bound(boost::make_tuple(tag, parent, fc::time_point_sec::maximum()));

        //        return get_discussions(query, tag, parent, tidx, tidx_itr, query.truncate_body);
    }

    std::vector<api::discussion> get_discussions_by_active(const api::discussion_query& query) const
    {
        return dummy();
    }

    std::vector<api::discussion> get_discussions_by_cashout(const api::discussion_query& query) const
    {
        return dummy();
    }

    std::vector<api::discussion> get_discussions_by_votes(const api::discussion_query& query) const
    {
        return dummy();
    }

    std::vector<api::discussion> get_discussions_by_children(const api::discussion_query& query) const
    {
        return dummy();
    }

    std::vector<api::discussion> get_discussions_by_hot(const api::discussion_query& query) const
    {
        return dummy();
    }

    std::vector<api::discussion> get_discussions_by_comments(const api::discussion_query& query) const
    {
        return dummy();
    }

    std::vector<api::discussion> get_discussions_by_promoted(const api::discussion_query& query) const
    {
        return dummy();
    }

    api::discussion get_content(const std::string& author, const std::string& permlink) const
    {
        api::discussion result;
        return result;
    }
    std::vector<api::discussion> get_content_replies(const std::string& parent,
                                                     const std::string& parent_permlink) const
    {
        return dummy();
    }

private:
    comment_id_type get_parent(const api::discussion_query& query) const
    {
        comment_id_type parent;

        if (query.parent_author && query.parent_permlink)
        {
            parent = get_parent(*query.parent_author, *query.parent_permlink);
        }

        return parent;
    }

    comment_id_type get_parent(const std::string& parent_author, const std::string& parent_permlink) const
    {
        return _services.comment_service().get(parent_author, parent_permlink).id;
    }

    void set_url(api::discussion& d) const
    {
        const api::comment_api_obj root(_services.comment_service().get(d.root_comment));
        d.url = "/" + root.category + "/@" + root.author + "/" + root.permlink;
        d.root_title = root.title;
        if (root.id != d.id)
            d.url += "#@" + d.author + "/" + d.permlink;
    }

    api::discussion get_discussion(comment_id_type id, uint32_t truncate_body) const
    {
        api::discussion d = _services.comment_service().get(id);

        set_url(d);
        set_pending_payout(d);

        d.active_votes = get_active_votes(d.author, d.permlink);
        d.body_length = d.body.size();

        if (truncate_body)
        {
            d.body = d.body.substr(0, truncate_body);

            if (!fc::is_utf8(d.body))
                d.body = fc::prune_invalid_utf8(d.body);
        }

        return d;
    }

    u256 to256(const fc::uint128& t) const
    {
        u256 result(t.high_bits());
        result <<= 65;
        result += t.low_bits();
        return result;
    }

    void set_pending_payout(api::discussion& d) const
    {
        _tags_service.set_promoted_balance(d.id, d.promoted);

        const auto& reward_fund_obj = _services.reward_fund_service().get();

        asset pot = reward_fund_obj.activity_reward_balance_scr;
        u256 total_r2 = to256(reward_fund_obj.recent_claims);
        if (total_r2 > 0)
        {
            uint128_t vshares;
            vshares = d.net_rshares.value > 0
                ? scorum::chain::util::evaluate_reward_curve(d.net_rshares.value, reward_fund_obj.author_reward_curve)
                : 0;

            u256 r2 = to256(vshares); // to256(abs_net_rshares);
            r2 *= pot.amount.value;
            r2 /= total_r2;

            d.pending_payout_value = asset(static_cast<uint64_t>(r2), pot.symbol());
        }

        if (d.parent_author != SCORUM_ROOT_POST_PARENT_ACCOUNT)
            d.cashout_time = _tags_service.calculate_discussion_payout_time(_services.comment_service().get(d.id));

        if (d.body.size() > 1024 * 128)
            d.body = "body pruned due to size";
        if (d.parent_author.size() > 0 && d.body.size() > 1024 * 16)
            d.body = "comment pruned due to size";

        set_url(d);
    }

    std::vector<api::vote_state> get_active_votes(const std::string& author, const std::string& permlink) const
    {
        std::vector<api::vote_state> result;
        const auto& comment = _services.comment_service().get(author, permlink);

        auto votes = _services.comment_vote_service().get_by_comment(comment.id);

        for (auto it = votes.begin(); it != votes.end(); ++it)
        {
            const auto comment_voute = it->get();
            const auto& vouter = _services.account_service().get(comment_voute.voter);

            api::vote_state vstate;
            vstate.voter = vouter.name;
            vstate.weight = comment_voute.weight;
            vstate.rshares = comment_voute.rshares;
            vstate.percent = comment_voute.vote_percent;
            vstate.time = comment_voute.last_update;

            result.push_back(vstate);
        }

        return result;
    }

    scorum::chain::data_service_factory_i& _services;
    tags_service_i& _tags_service;
};

} // namespace tags
} // namespace scorum
