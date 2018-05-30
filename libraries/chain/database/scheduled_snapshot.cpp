#include <scorum/chain/database/scheduled_snapshot.hpp>

#include <scorum/chain/database/database.hpp>
#include <chainbase/db_state.hpp>

#include <scorum/chain/services/dynamic_global_property.hpp>
#include <scorum/chain/services/snapshot.hpp>

#include <string>
#include <sstream>
#include <fstream>

#include <fc/io/raw.hpp>

#include <scorum/snapshot/loader.hpp>
#include <scorum/snapshot/saver.hpp>
#include <scorum/chain/database/snapshot_types.hpp>

namespace scorum {
namespace chain {
namespace database_ns {

using db_state = chainbase::db_state;
using scorum::snapshot::base_section;
using scorum::chain::by_id;

class scheduled_snapshot_impl
{
public:
    scheduled_snapshot_impl(database& db, db_state& state)
        : dprops_service(db.dynamic_global_property_service())
        , _state(state)
    {
    }

    fc::path get_snapshot_path(const fc::path& snapshot_dir)
    {
        std::stringstream snapshot_name;
        snapshot_name << dprops_service.get().head_block_number;
        snapshot_name << "-";
        snapshot_name << dprops_service.get().time.to_iso_string();
        snapshot_name << ".bin";

        return snapshot_dir / snapshot_name.str();
    }

    void save(const fc::path& snapshot_path, database_virtual_operations_emmiter_i& vops)
    {
        fc::remove_all(snapshot_path);

        std::ofstream snapshot_stream;
        snapshot_stream.open(snapshot_path.generic_string(), std::ios::binary);

        block_id_type snapshot_block_id = dprops_service.get().head_block_id;
        fc::raw::pack(snapshot_stream, snapshot_block_id);

        scorum::snapshot::save_index_section<by_id>(snapshot_stream, _state, base_section());

        vops.notify_save_snapshot(snapshot_stream);
        snapshot_stream.close();
    }

    block_id_type load_header(const fc::path& snapshot_path)
    {
        std::ifstream snapshot_stream;
        snapshot_stream.open(snapshot_path.generic_string(), std::ios::binary);

        block_id_type snapshot_block_id;
        fc::raw::unpack(snapshot_stream, snapshot_block_id);

        snapshot_stream.close();

        return snapshot_block_id;
    }

    void load(const fc::path& snapshot_path, database_virtual_operations_emmiter_i& vops)
    {
        std::ifstream snapshot_stream;
        snapshot_stream.open(snapshot_path.generic_string(), std::ios::binary);

        block_id_type snapshot_block_id;
        fc::raw::unpack(snapshot_stream, snapshot_block_id);

        scorum::snapshot::load_index_section(snapshot_stream, _state, base_section());

        vops.notify_load_snapshot(snapshot_stream);
        snapshot_stream.close();
    }

private:
    dynamic_global_property_service_i& dprops_service;

    db_state& _state;
};

make_scheduled_snapshot::make_scheduled_snapshot(database& db)
    : _impl(new scheduled_snapshot_impl(db, static_cast<db_state&>(db)))
{
}
make_scheduled_snapshot::~make_scheduled_snapshot()
{
}

void make_scheduled_snapshot::on_apply(block_task_context& ctx)
{
    snapshot_service_i& snapshot_service = ctx.services().snapshot_service();

    if (!snapshot_service.is_snapshot_scheduled())
        return;

    fc::path snapshot_dir = snapshot_service.get_snapshot_dir();

    _impl->save(_impl->get_snapshot_path(snapshot_dir), static_cast<database_virtual_operations_emmiter_i&>(ctx));

    snapshot_service.clear_snapshot_schedule();
}

load_scheduled_snapshot::load_scheduled_snapshot(database& db)
    : _impl(new scheduled_snapshot_impl(db, static_cast<db_state&>(db)))
    , _db(db)
{
}
load_scheduled_snapshot::~load_scheduled_snapshot()
{
}

block_id_type load_scheduled_snapshot::load_header(const fc::path& snapshot_path)
{
    return _impl->load_header(snapshot_path);
}

void load_scheduled_snapshot::load(const fc::path& snapshot_path)
{
    _impl->load(snapshot_path, static_cast<database_virtual_operations_emmiter_i&>(_db));
}
}
}
}
