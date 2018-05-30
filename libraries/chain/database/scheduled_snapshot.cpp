#include <scorum/chain/database/scheduled_snapshot.hpp>

#include <scorum/chain/database/database.hpp>
#include <chainbase/db_state.hpp>

#include <scorum/chain/services/dynamic_global_property.hpp>
#include <scorum/chain/services/snapshot.hpp>

#include <scorum/chain/database/snapshot_types.hpp>

#include <string>
#include <sstream>
#include <fstream>

#include <fc/io/raw.hpp>
#include <fc/crypto/ripemd160.hpp>

namespace scorum {
namespace chain {
namespace database_ns {

using db_state = chainbase::db_state;

class save_index_visitor
{
public:
    using result_type = void;

    save_index_visitor(std::ofstream& fstream, db_state& state)
        : _fstream(fstream)
        , _state(state)
    {
    }

    template <class T> void operator()(const T&) const
    {
        using object_type = typename T::type;
        const auto& index = _state.template get_index<typename chainbase::get_index_type<object_type>::type>()
                                .indices()
                                .template get<by_id>();
        fc::raw::pack(_fstream, index.size());
        for (auto itr = index.begin(); itr != index.end(); ++itr)
        {
            const object_type& obj = (*itr);
            fc::raw::pack(_fstream, obj);
        }
    }

private:
    std::ofstream& _fstream;
    db_state& _state;
};

class load_index_visitor
{
public:
    using result_type = void;

    load_index_visitor(std::ifstream& fstream, db_state& state)
        : _fstream(fstream)
        , _state(state)
    {
    }

    template <class T> void operator()(const T&) const
    {
        using object_type = typename T::type;
        FC_ASSERT(_state.template find<object_type>() == nullptr, "State is not empty");
        size_t sz = 0;
        fc::raw::unpack(_fstream, sz);
        for (size_t ci = 0; ci < sz; ++ci)
        {
            _state.template create<object_type>([&](object_type& obj) { fc::raw::unpack(_fstream, obj); });
        }
    }

private:
    std::ifstream& _fstream;
    db_state& _state;
};

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

    void save_snapshot(const fc::path& snapshot_path, database_virtual_operations_emmiter_i& vops)
    {
        fc::remove_all(snapshot_path);

        std::ofstream snapshot_stream;
        snapshot_stream.open(snapshot_path.generic_string(), std::ios::binary);

        block_id_type snapshot_block_id = dprops_service.get().head_block_id;
        fc::raw::pack(snapshot_stream, snapshot_block_id);

        fc::ripemd160::encoder check_enc;
        fc::raw::pack(check_enc, scorum::get_extraction_section().name);
        fc::raw::pack(snapshot_stream, check_enc.result());

        _state.for_each_index_key([&](int index_id) {
            auto v = scorum::get_object_type_variant(index_id);
            v.visit(save_index_visitor(snapshot_stream, _state));
        });

        vops.save_snapshot(snapshot_stream);
        snapshot_stream.close();
    }

    block_id_type load_snapshot_header(const fc::path& snapshot_path)
    {
        std::ifstream snapshot_stream;
        snapshot_stream.open(snapshot_path.generic_string(), std::ios::binary);

        block_id_type snapshot_block_id;
        fc::raw::unpack(snapshot_stream, snapshot_block_id);

        snapshot_stream.close();

        return snapshot_block_id;
    }

    void load_snapshot(const fc::path& snapshot_path, database_virtual_operations_emmiter_i& vops)
    {
        std::ifstream snapshot_stream;
        snapshot_stream.open(snapshot_path.generic_string(), std::ios::binary);

        block_id_type snapshot_block_id;
        fc::raw::unpack(snapshot_stream, snapshot_block_id);

        fc::ripemd160 check;

        fc::raw::unpack(snapshot_stream, check);

        fc::ripemd160::encoder check_enc;
        fc::raw::pack(check_enc, scorum::get_extraction_section().name);

        FC_ASSERT(check_enc.result() == check);

        _state.for_each_index_key([&](int index_id) {
            auto v = scorum::get_object_type_variant(index_id);
            v.visit(load_index_visitor(snapshot_stream, _state));
        });

        vops.load_snapshot(snapshot_stream);
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

    _impl->save_snapshot(_impl->get_snapshot_path(snapshot_dir),
                         static_cast<database_virtual_operations_emmiter_i&>(ctx));

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
    return _impl->load_snapshot_header(snapshot_path);
}

void load_scheduled_snapshot::load(const fc::path& snapshot_path)
{
    _impl->load_snapshot(snapshot_path, static_cast<database_virtual_operations_emmiter_i&>(_db));
}
}
}
}
