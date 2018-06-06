#pragma once

#include <scorum/chain/database/block_tasks/block_tasks.hpp>

#include <scorum/protocol/types.hpp>

#include <scorum/chain/services/dynamic_global_property.hpp>

namespace scorum {
namespace chain {

class database;

namespace database_ns {

class scheduled_snapshot_impl;

using scorum::protocol::digest_type;

struct snapshot_header
{
    uint8_t version = 0;
    uint32_t head_block_number = 0;
    digest_type head_block_digest;
    uint32_t chainbase_flags = 0;
};

struct save_scheduled_snapshot : public block_task
{
    explicit save_scheduled_snapshot(database& db);
    ~save_scheduled_snapshot();

    static fc::path get_snapshot_path(dynamic_global_property_service_i&, const fc::path& snapshot_dir);

    virtual void on_apply(block_task_context&);

    void check_snapshot_task(uint32_t block_number, const fc::path& task_dir);

private:
    std::unique_ptr<scheduled_snapshot_impl> _impl;
    database& _db;
};

struct load_scheduled_snapshot
{
    explicit load_scheduled_snapshot(database& db);
    ~load_scheduled_snapshot();

    snapshot_header load_header(const fc::path& snapshot_path);
    void load(const fc::path& snapshot_path);

private:
    std::unique_ptr<scheduled_snapshot_impl> _impl;
    database& _db;
};
}
}
}

FC_REFLECT(scorum::chain::database_ns::snapshot_header,
           (version)(head_block_number)(head_block_digest)(chainbase_flags))
