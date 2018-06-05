#pragma once

#include <scorum/protocol/operations.hpp>
#include <scorum/protocol/scorum_virtual_operations.hpp>
#include <scorum/snapshot/get_types_by_id.hpp>

namespace scorum {
namespace chain {

using scorum::protocol::operation;

struct database_virtual_operations_emmiter_i
{
    virtual void push_virtual_operation(const operation& op) = 0;
    virtual void notify_save_snapshot(std::ofstream&) = 0;
    virtual void notify_load_snapshot(std::ifstream&, scorum::snapshot::index_ids_type&) = 0;
};
}
}
