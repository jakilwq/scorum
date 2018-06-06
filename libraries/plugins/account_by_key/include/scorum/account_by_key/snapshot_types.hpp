#pragma once

#include <scorum/snapshot/get_types_by_id.hpp>

#include <scorum/account_by_key/account_by_key_objects.hpp>

using namespace scorum::account_by_key;

// clang-format off
SCORUM_OBJECT_TYPES_FOR_SNAPSHOT_SECTION(account_by_key_section, account_by_key_object_types,
                              (key_lookup)
                              )
// clang-format on
