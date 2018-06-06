#pragma once

#include <scorum/chain/database/block_tasks/block_tasks.hpp>

namespace scorum {
namespace chain {
namespace database_ns {

struct process_funds : public block_task
{
    virtual void on_apply(block_task_context&);

    bool apply_schedule_crutches(block_task_context&);
};
}
}
}
