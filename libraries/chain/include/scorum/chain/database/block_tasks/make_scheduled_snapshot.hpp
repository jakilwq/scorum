#pragma once

#include <scorum/chain/database/block_tasks/block_tasks.hpp>

namespace scorum {
namespace chain {
namespace database_ns {

struct make_scheduled_snapshot : public block_task
{
    virtual void on_apply(block_task_context&);

private:
    fc::path get_snapshot_path(const fc::path& snapshot_dir);
    void make_snapshot(const fc::path& snapshot_path);
};
}
}
}
