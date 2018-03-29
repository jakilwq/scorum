#pragma once

#include <scorum/app/api_context.hpp>
#include <fc/api.hpp>

namespace scorum {
namespace app {

class comments_api : public std::enable_shared_from_this<comments_api>
{
public:
    comments_api(const api_context& a);

private:
    application& _app;
};

} // namespace app
} // namespace scorum
