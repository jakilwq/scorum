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
bool dbs_snapshot::is_snapshot_scheduled() const
{
    return db_impl().is_snapshot_scheduled();
}
void dbs_snapshot::clear_snapshot_schedule()
{
    return db_impl().clear_snapshot_schedule();
}
}
}
