#include <scorum/chain/services/snapshot.hpp>

#include <scorum/chain/database/database.hpp>

using namespace scorum::protocol;

namespace scorum {
namespace chain {

dbs_snapshot::dbs_snapshot(database& db)
    : dbs_base(db)
{
}

fc::path dbs_snapshot::get_snapshot_dir() const
{
    return db_impl().snapshot_dir();
}
void dbs_snapshot::schedule_snapshot_task(uint32_t number /*= 0*/)
{
    return db_impl().schedule_snapshot_task(number);
}
bool dbs_snapshot::is_snapshot_scheduled() const
{
    return db_impl().is_snapshot_scheduled();
}
uint32_t dbs_snapshot::get_snapshot_scheduled_number() const
{
    return db_impl().get_snapshot_scheduled_number();
}
void dbs_snapshot::clear_snapshot_schedule()
{
    return db_impl().clear_snapshot_schedule();
}
}
}
