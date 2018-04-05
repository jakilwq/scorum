#include <scorum/chain/evaluators/comment_evaluator.hpp>

#include <scorum/chain/schema/scorum_objects.hpp>

#include <scorum/chain/data_service_factory.hpp>
#include <scorum/chain/services/account.hpp>
#include <scorum/chain/services/dynamic_global_property.hpp>
#include <scorum/chain/services/comment.hpp>

#ifndef IS_LOW_MEM
#include <diff_match_patch.h>
#include <boost/locale/encoding_utf.hpp>

using boost::locale::conv::utf_to_utf;

std::wstring utf8_to_wstring(const std::string& str)
{
    return utf_to_utf<wchar_t>(str.c_str(), str.c_str() + str.size());
}

std::string wstring_to_utf8(const std::wstring& str)
{
    return utf_to_utf<char>(str.c_str(), str.c_str() + str.size());
}

#endif

namespace scorum {
namespace chain {

struct strcmp_equal
{
    bool operator()(const fc::shared_string& a, const std::string& b)
    {
        return a.size() == b.size() || std::strcmp(a.c_str(), b.c_str()) == 0;
    }
};

inline void validate_permlink_0_1(const std::string& permlink)
{
    FC_ASSERT(permlink.size() > SCORUM_MIN_PERMLINK_LENGTH && permlink.size() < SCORUM_MAX_PERMLINK_LENGTH,
              "Permlink is not a valid size.");

    for (auto ch : permlink)
    {
        if (!std::islower(ch) && !std::isdigit(ch) && !(ch == '-'))
        {
            FC_ASSERT(false, "Invalid permlink character: ${ch}", ("ch", std::string(1, ch)));
        }
    }
}

comment_evaluator::comment_evaluator(data_service_factory_i& services)
    : evaluator_impl<data_service_factory_i, comment_evaluator>(services)
    , _account_service(services.account_service())
    , _comment_service(services.comment_service())
    , _properties_service(services.dynamic_global_property_service())
{
}

void comment_evaluator::do_apply(const comment_operation& o)
{
    account_service_i& account_service = db().account_service();
    comment_service_i& comment_service = db().comment_service();
    dynamic_global_property_service_i& dprops_service = db().dynamic_global_property_service();

    try
    {

        FC_ASSERT(o.title.size() + o.body.size() + o.json_metadata.size(),
                  "Cannot update comment because nothing appears to be changing.");

        const auto& auth = account_service.get_account(o.author); /// prove it exists

        FC_ASSERT(!(auth.owner_challenged || auth.active_challenged),
                  "Operation cannot be processed because account is currently challenged.");

        account_name_type parent_author = o.parent_author;
        std::string parent_permlink = o.parent_permlink;
        if (parent_author != SCORUM_ROOT_POST_PARENT_ACCOUNT)
        {
            const comment_object& parent = comment_service.get(parent_author, parent_permlink);
            FC_ASSERT(parent.depth < SCORUM_MAX_COMMENT_DEPTH,
                      "Comment is nested ${x} posts deep, maximum depth is ${y}.",
                      ("x", parent.depth)("y", SCORUM_MAX_COMMENT_DEPTH));
        }

        if (o.json_metadata.size())
            FC_ASSERT(fc::is_utf8(o.json_metadata), "JSON Metadata must be UTF-8");

        auto now = dprops_service.head_block_time();

        if (!comment_service.is_exists(o.author, o.permlink))
        {
            if (parent_author != SCORUM_ROOT_POST_PARENT_ACCOUNT)
            {
                const comment_object& parent = comment_service.get(parent_author, parent_permlink);
                FC_ASSERT(comment_service.get(parent.root_comment).allow_replies,
                          "The parent comment has disabled replies.");
            }

            if (parent_author == SCORUM_ROOT_POST_PARENT_ACCOUNT)
                FC_ASSERT((now - auth.last_root_post) > SCORUM_MIN_ROOT_COMMENT_INTERVAL,
                          "You may only post once every 5 minutes.",
                          ("now", now)("last_root_post", auth.last_root_post));
            else
                FC_ASSERT((now - auth.last_post) > SCORUM_MIN_REPLY_INTERVAL,
                          "You may only comment once every 20 seconds.",
                          ("now", now)("auth.last_post", auth.last_post));

            uint16_t reward_weight = SCORUM_100_PERCENT;

            account_service.add_post(auth, parent_author);

            validate_permlink_0_1(parent_permlink);
            validate_permlink_0_1(o.permlink);

            account_name_type pr_parent_author;
            std::string pr_parent_permlink;
            uint16_t pr_depth = 0;
            std::string pr_category;
            comment_id_type pr_root_comment;
            if (parent_author != SCORUM_ROOT_POST_PARENT_ACCOUNT)
            {
                const comment_object& parent = comment_service.get(parent_author, parent_permlink);
                pr_parent_author = parent.author;
                pr_parent_permlink = fc::to_string(parent.permlink);
                pr_depth = parent.depth + 1;
                pr_category = fc::to_string(parent.category);
                pr_root_comment = parent.root_comment;
            }

            comment_service.create([&](comment_object& com) {

                com.author = o.author;
                fc::from_string(com.permlink, o.permlink);
                com.last_update = now;
                com.created = com.last_update;
                com.active = com.last_update;
                com.last_payout = fc::time_point_sec::min();
                com.max_cashout_time = fc::time_point_sec::maximum();
                com.reward_weight = reward_weight;

                if (parent_author == SCORUM_ROOT_POST_PARENT_ACCOUNT)
                {
                    com.parent_author = "";
                    fc::from_string(com.parent_permlink, parent_permlink);
                    fc::from_string(com.category, parent_permlink);
                    com.root_comment = com.id;
                }
                else
                {
                    com.parent_author = pr_parent_author;
                    fc::from_string(com.parent_permlink, pr_parent_permlink);
                    com.depth = pr_depth + 1;
                    fc::from_string(com.category, pr_category);
                    com.root_comment = pr_root_comment;
                }

                com.cashout_time = com.created + SCORUM_CASHOUT_WINDOW_SECONDS;

#ifndef IS_LOW_MEM
                fc::from_string(com.title, o.title);
                if (o.body.size() < 1024 * 1024 * 128)
                {
                    fc::from_string(com.body, o.body);
                }
                if (fc::is_utf8(o.json_metadata))
                    fc::from_string(com.json_metadata, o.json_metadata);
                else
                    wlog("Comment ${a}/${p} contains invalid UTF-8 metadata", ("a", o.author)("p", o.permlink));
#endif
            });

            /// this loop can be skiped for validate-only nodes as it is merely gathering stats for indices
            while (parent_author != SCORUM_ROOT_POST_PARENT_ACCOUNT)
            {
                const comment_object& parent = comment_service.get(parent_author, parent_permlink);

                parent_author = parent.parent_author;
                parent_permlink = fc::to_string(parent.parent_permlink);

                comment_service.update(parent, [&](comment_object& p) {
                    p.children++;
                    p.active = now;
                });
#ifdef IS_LOW_MEM
                break;
#endif
            }
        }
        else // start edit case
        {
            const comment_object& comment = comment_service.get(o.author, o.permlink);

            comment_service.update(comment, [&](comment_object& com) {
                com.last_update = dprops_service.head_block_time();
                com.active = com.last_update;
                strcmp_equal equal;

                if (parent_author == SCORUM_ROOT_POST_PARENT_ACCOUNT)
                {
                    FC_ASSERT(com.parent_author == account_name_type(), "The parent of a comment cannot be changed.");
                    FC_ASSERT(equal(com.parent_permlink, parent_permlink), "The permlink of a comment cannot change.");
                }
                else
                {
                    FC_ASSERT(com.parent_author == o.parent_author, "The parent of a comment cannot be changed.");
                    FC_ASSERT(equal(com.parent_permlink, parent_permlink), "The permlink of a comment cannot change.");
                }

#ifndef IS_LOW_MEM
                if (o.title.size())
                    fc::from_string(com.title, o.title);
                if (o.json_metadata.size())
                {
                    if (fc::is_utf8(o.json_metadata))
                        fc::from_string(com.json_metadata, o.json_metadata);
                    else
                        wlog("Comment ${a}/${p} contains invalid UTF-8 metadata", ("a", o.author)("p", o.permlink));
                }

                if (o.body.size())
                {
                    try
                    {
                        diff_match_patch<std::wstring> dmp;
                        auto patch = dmp.patch_fromText(utf8_to_wstring(o.body));
                        if (patch.size())
                        {
                            auto result = dmp.patch_apply(patch, utf8_to_wstring(fc::to_string(com.body)));
                            auto patched_body = wstring_to_utf8(result.first);
                            if (!fc::is_utf8(patched_body))
                            {
                                idump(("invalid utf8")(patched_body));
                                fc::from_string(com.body, fc::prune_invalid_utf8(patched_body));
                            }
                            else
                            {
                                fc::from_string(com.body, patched_body);
                            }
                        }
                        else
                        { // replace
                            fc::from_string(com.body, o.body);
                        }
                    }
                    catch (...)
                    {
                        fc::from_string(com.body, o.body);
                    }
                }
#endif
            });

        } // end EDIT case
    }
    FC_CAPTURE_AND_RETHROW((o))
}

} // namespace chain
} // namespace scorum
