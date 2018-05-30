#pragma once

#include <scorum/chain/database/block_tasks/block_tasks.hpp>

#include <scorum/protocol/types.hpp>

namespace scorum {
namespace chain {

class database;

namespace database_ns {

class scheduled_snapshot_impl;

struct make_scheduled_snapshot : public block_task
{
    explicit make_scheduled_snapshot(database& db);
    ~make_scheduled_snapshot();

    virtual void on_apply(block_task_context&);

private:
    std::unique_ptr<scheduled_snapshot_impl> _impl;
};

using scorum::protocol::block_id_type;

struct load_scheduled_snapshot
{
    explicit load_scheduled_snapshot(database& db);
    ~load_scheduled_snapshot();

    block_id_type load(const fc::path& snapshot_path);

private:
    std::unique_ptr<scheduled_snapshot_impl> _impl;
    database& _db;
};
}
}
}
