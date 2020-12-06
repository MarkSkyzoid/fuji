#pragma once

// USAGE: set API in CMake

#define FUJI_API_VULKAN 0
#define FUJI_API_DX12   1

#if FUJI_API != FUJI_API_VULKAN
#error API not implemented
#endif // FUJI_API

// Api implementation here
namespace fuji
{
    struct Context {

    };
}
