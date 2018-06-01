#include <scorum/chain/database/scheduled_snapshot.hpp>

#include <scorum/chain/database/database.hpp>
#include <chainbase/db_state.hpp>

#include <scorum/chain/services/dynamic_global_property.hpp>
#include <scorum/chain/services/snapshot.hpp>
#include <scorum/chain/services/blocks_story.hpp>

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
        , blocks_story_service(db.blocks_story_service())
        , _state(state)
    {
    }

    fc::path get_snapshot_path(const fc::path& snapshot_dir)
    {
        std::stringstream snapshot_name;
        snapshot_name << dprops_service.get().time.to_iso_string();
        snapshot_name << "-";
        snapshot_name << dprops_service.get().head_block_number;
        snapshot_name << ".bin";

        return snapshot_dir / snapshot_name.str();
    }

    void save(const fc::path& snapshot_path, database_virtual_operations_emmiter_i& vops)
    {
        fc::remove_all(snapshot_path);

        std::ofstream snapshot_stream;
        snapshot_stream.open(snapshot_path.generic_string(), std::ios::binary);

        optional<signed_block> b = blocks_story_service.fetch_block_by_id(dprops_service.get().head_block_id);

        FC_ASSERT(b.valid(), "Invalid block header");

        snapshot_header header;
        header.head_block_number = b->block_num();
        header.head_block_digest = b->digest();
        header.chainbase_flags = chainbase::database::read_write;
        fc::raw::pack(snapshot_stream, header);

        scorum::snapshot::save_index_section<by_id>(snapshot_stream, _state, base_section());

        vops.notify_save_snapshot(snapshot_stream);
        snapshot_stream.close();
    }

    snapshot_header load_header(const fc::path& snapshot_path)
    {
        std::ifstream snapshot_stream;
        snapshot_stream.open(snapshot_path.generic_string(), std::ios::binary);

        snapshot_header header = load_header(snapshot_stream);

        snapshot_stream.close();

        return header;
    }

    snapshot_header load_header(std::ifstream& fs)
    {
        snapshot_header header;
        fc::raw::unpack(fs, header);

        return header;
    }

    void load(const fc::path& snapshot_path, database_virtual_operations_emmiter_i& vops)
    {
        std::ifstream snapshot_stream;
        snapshot_stream.open(snapshot_path.generic_string(), std::ios::binary);

        uint64_t sz = fc::file_size(snapshot_path);

        std::cerr << sz << std::endl;

        snapshot_header header = load_header(snapshot_stream);

        ilog("Loading snapshot for block ${n} from file ${f}.",
             ("n", header.head_block_number)("f", snapshot_path.generic_string()));

        scorum::snapshot::load_index_section(snapshot_stream, _state, base_section());

        vops.notify_load_snapshot(snapshot_stream);

        std::cerr << snapshot_stream.tellg() << std::endl;

        FC_ASSERT((uint64_t)snapshot_stream.tellg() != sz,
                  "Not all indexes are loaded. Node configuration does not match snapshot.");

        snapshot_stream.close();
    }

private:
    dynamic_global_property_service_i& dprops_service;
    blocks_story_service_i& blocks_story_service;

    db_state& _state;
};

save_scheduled_snapshot::save_scheduled_snapshot(database& db)
    : _impl(new scheduled_snapshot_impl(db, static_cast<db_state&>(db)))
{
}
save_scheduled_snapshot::~save_scheduled_snapshot()
{
}

void save_scheduled_snapshot::on_apply(block_task_context& ctx)
{
    snapshot_service_i& snapshot_service = ctx.services().snapshot_service();

    if (!snapshot_service.is_snapshot_scheduled())
        return;

    snapshot_service.clear_snapshot_schedule();

    fc::path snapshot_path = _impl->get_snapshot_path(snapshot_service.get_snapshot_dir());
    try
    {
        ilog("Making snapshot for block ${n} to file ${f}.",
             ("n", ctx.block_num())("f", snapshot_path.generic_string()));
        _impl->save(snapshot_path, static_cast<database_virtual_operations_emmiter_i&>(ctx));
        return;
    }
    FC_CAPTURE_AND_LOG((ctx.block_num())(snapshot_path))
    fc::remove_all(snapshot_path);
}

load_scheduled_snapshot::load_scheduled_snapshot(database& db)
    : _impl(new scheduled_snapshot_impl(db, static_cast<db_state&>(db)))
    , _db(db)
{
}
load_scheduled_snapshot::~load_scheduled_snapshot()
{
}

snapshot_header load_scheduled_snapshot::load_header(const fc::path& snapshot_path)
{
    try
    {
        return _impl->load_header(snapshot_path);
    }
    FC_CAPTURE_AND_RETHROW((snapshot_path))
}

void load_scheduled_snapshot::load(const fc::path& snapshot_path)
{
    try
    {
        _impl->load(snapshot_path, static_cast<database_virtual_operations_emmiter_i&>(_db));
    }
    FC_CAPTURE_AND_RETHROW((snapshot_path))
}
}
}
}
