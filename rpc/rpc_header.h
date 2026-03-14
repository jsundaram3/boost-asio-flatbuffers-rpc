#pragma once
#include <cstdint>

struct RpcHeader
{
    uint32_t size;
    uint16_t method;
    uint32_t request_id;
};