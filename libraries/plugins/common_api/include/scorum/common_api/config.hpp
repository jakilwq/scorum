#pragma once

#include <fc/time.hpp>

namespace scorum {

static const uint32_t MAX_BLOCKCHAIN_HISTORY_DEPTH = 100;
static const uint32_t MAX_BLOCKS_HISTORY_DEPTH = 100;
static const uint32_t MAX_BUDGETS_LIST_SIZE = 100;
static const uint32_t LOOKUP_LIMIT = 1000;

static const int64_t TAG_LIFETIME_AFTER_CASHOUT_SEC = fc::days(7).to_seconds();
}
