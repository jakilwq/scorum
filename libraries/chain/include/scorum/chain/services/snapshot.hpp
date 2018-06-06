#pragma once

#include <scorum/chain/services/service_base.hpp>

namespace scorum {
namespace chain {

struct snapshot_service_i
{
    virtual fc::path get_snapshot_dir() const = 0;
    virtual bool is_snapshot_scheduled() const = 0;
    virtual void clear_snapshot_schedule() = 0;
};

class dbs_snapshot : public dbs_base, public snapshot_service_i
{
    friend class dbservice_dbs_factory;

protected:
    explicit dbs_snapshot(database& db);

    virtual fc::path get_snapshot_dir() const override;
    virtual bool is_snapshot_scheduled() const override;
    virtual void clear_snapshot_schedule() override;
};

} // namespace scorum
} // namespace chain
