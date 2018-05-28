#include <scorum/chain/database/block_tasks/make_scheduled_snapshot.hpp>

#include <scorum/chain/services/dynamic_global_property.hpp>
#include <scorum/chain/services/snapshot.hpp>

#include <string>

namespace scorum {
namespace chain {
namespace database_ns {

void make_scheduled_snapshot::on_apply(block_task_context& ctx)
{
    snapshot_service_i& snapshot_service = ctx.services().snapshot_service();

    if (!snapshot_service.is_snapshot_scheduled())
        return;

    fc::path snapshot_dir = snapshot_service.get_snapshot_dir();

    make_snapshot(get_snapshot_path(snapshot_dir));

    snapshot_service.clear_snapshot_schedule();
}

fc::path make_scheduled_snapshot::get_snapshot_path(const fc::path& snapshot_dir)
{
    std::string snapshot_name;

    // TODO

    return snapshot_dir / snapshot_name;
}

void make_scheduled_snapshot::make_snapshot(const fc::path&)
{
    // TODO
}
}
}
}
