#pragma once
#include <functional>
#include <unordered_map>
#include <vector>
#include <cstdint>

struct RpcRequestContext
{
    uint32_t request_id;
    std::vector<uint8_t> payload;
};

using RpcHandler = std::function<std::vector<uint8_t>(const RpcRequestContext&)>;

class RpcDispatcher
{
    std::unordered_map<uint16_t,RpcHandler> handlers_;
public:
    void register_handler(uint16_t method_id, RpcHandler handler)
    {
        handlers_[method_id] = handler;
    }

    std::vector<uint8_t> dispatch(uint16_t method_id, const RpcRequestContext& ctx)
    {
        return handlers_[method_id](ctx);
    }
};