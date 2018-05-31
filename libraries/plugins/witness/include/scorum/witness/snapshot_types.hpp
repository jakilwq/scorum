#pragma once

#include <scorum/snapshot/get_types_by_id.hpp>

#include <scorum/witness/witness_objects.hpp>

using namespace scorum::witness;

// clang-format off
SCORUM_OBJECT_TYPES_FOR_SNAPSHOT_SECTION(witness_section, witness_plugin_object_type,
                              (account_bandwidth)
                              (reserve_ratio)
                              )
// clang-format on
