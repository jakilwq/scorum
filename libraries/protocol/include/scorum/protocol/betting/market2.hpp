#pragma once
#include <scorum/protocol/betting/wincase.hpp>
#include <scorum/protocol/betting/wincase_comparison.hpp>

#include <type_traits>
#include <boost/preprocessor/seq/for_each.hpp>
//#include <boost/preprocessor/

namespace scorum {
namespace protocol {
namespace betting {
namespace next {

namespace wincases {

struct over;
struct under;
struct yes;
struct no;

struct home;
struct draw_or_away;

struct draw;
struct home_or_away;

struct away;
struct home_or_draw;

template <typename First, typename Second> struct wincase_pair
{
    typedef First first_type;
    typedef Second second_type;
};

using over_under = wincase_pair<over, under>;
using yes_no = wincase_pair<yes, no>;

using home_x_draw_or_away = wincase_pair<home, draw_or_away>;
using draw_x_home_or_away = wincase_pair<home, home_or_away>;
using away_x_home_or_draw = wincase_pair<home, home_or_draw>;

template <typename Market, typename Wincase> struct wincase
{
    wincase(Market m)
        : market(m)
    {
    }

private:
    Market market;
};

} // namespace wincases

namespace markets {

struct score
{
    uint16_t home;
    uint16_t away;
};

struct total
{
    threshold_type threshold = 0;
};

struct handicap
{
    threshold_type threshold = 0;
};

struct result
{
};

struct round
{
};

struct winner
{
};

template <typename Market, typename... Wincases> struct market_base
{
    using market_wincases = fc::static_variant<Wincases...>;

    market_base(Market market)
        : _market(market)
    {
    }

    template <typename Wincase> auto wincase()
    {
        return wincases::wincase<market_base<Market, Wincases...>, Wincase>(*this);
    }

private:
    Market _market;
    fc::flat_set<market_wincases> wincases;
};

// clang-format off
using handicap_market = market_base<markets::handicap, wincases::over_under>;
using total_market = market_base<markets::total, wincases::over_under>;
using winner_market = market_base<markets::winner, wincases::home_x_draw_or_away,
                                                   wincases::draw_x_home_or_away,
                                                   wincases::away_x_home_or_draw>;

using markets = fc::static_variant<total_market,
                                   winner_market>;

// clang-format on

} // namespace markets

} // namespace next
} // namespace betting

struct create_game_op
{
    std::vector<betting::next::markets::markets> markets;
};

struct post_bet_op
{
    //    betting::next::markets::wincases wincase;
};

struct post_game_results_op
{
};

} // namespace protocol
} // namespace scorum

// FC_REFLECT(scorum::protocol::betting::next::wincases::over, )
// FC_REFLECT(scorum::protocol::betting::next::wincases::under, )
// FC_REFLECT(scorum::protocol::betting::next::wincases::yes, )
// FC_REFLECT(scorum::protocol::betting::next::wincases::no, )
// FC_REFLECT(scorum::protocol::betting::next::wincases::home, )
// FC_REFLECT(scorum::protocol::betting::next::wincases::draw_or_away, )
// FC_REFLECT(scorum::protocol::betting::next::wincases::draw, )
// FC_REFLECT(scorum::protocol::betting::next::wincases::home_or_away, )
// FC_REFLECT(scorum::protocol::betting::next::wincases::away, )
// FC_REFLECT(scorum::protocol::betting::next::wincases::home_or_draw, )
// FC_REFLECT(scorum::protocol::betting::next::wincases::over_under, )
// FC_REFLECT(scorum::protocol::betting::next::wincases::yes_no, )
// FC_REFLECT(scorum::protocol::betting::next::wincases::home_x_draw_or_away, )
// FC_REFLECT(scorum::protocol::betting::next::wincases::draw_x_home_or_away, )
// FC_REFLECT(scorum::protocol::betting::next::wincases::away_x_home_or_draw, )

// FC_REFLECT(scorum::protocol::betting::next::wincases::wincase_pair, )

FC_REFLECT(scorum::protocol::betting::next::markets::score, (home)(away))
FC_REFLECT(scorum::protocol::betting::next::markets::total, (threshold))
FC_REFLECT(scorum::protocol::betting::next::markets::handicap, (threshold))

FC_REFLECT(scorum::protocol::betting::next::markets::total_market, )
FC_REFLECT(scorum::protocol::betting::next::markets::winner_market, )

FC_REFLECT(scorum::protocol::betting::next::markets::markets, )
FC_REFLECT(scorum::protocol::create_game_op, (markets))
