#pragma once
//#include <boost
#include <boost/mpl/vector.hpp>

struct total;
struct under;

struct home;
struct draw_or_away;

struct draw;
struct home_or_away;

struct away;
struct home_or_draw;

// clang-format off

//struct xxx : boost::mpl::vector<
//    market<market_total, wincase_pair<total, under>>,

//    market<market_winner, wincase_pair<home, draw_or_away>,
//                          wincase_pair<draw, home_or_away>>
//        >
//{
//};

// clang-format on
