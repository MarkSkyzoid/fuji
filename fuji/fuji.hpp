#pragma once

// USAGE: set API in CMake
#define FUJI_API_VULKAN 0
#define FUJI_API_DX12 1

#if FUJI_API != FUJI_API_VULKAN
#error API not implemented
#endif // FUJI_API

#if defined(_WIN32)
#define FUJI_WIN32
// Pull in minimal Windows headers
#if !defined(NOMINMAX)
#define NOMINMAX
#endif
#if !defined(WIN32_LEAN_AND_MEAN)
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#endif

#include <stdint.h>

// Api implementation here
namespace fuji {
	// Constants
	constexpr uint32_t MAX_SWAPCHAIN_IMAGE_COUNT = 3;

	// Structures
	enum class Format : uint32_t
	{
		Undefined = 0,

		// 1 channel
		R8_UNORM,
		R16_UNORM,
		R16_FLOAT,
		R32_UINT,
		R32_FLOAT,

		// 2 channels
		R8G8_UNORM,
		R16G16_UNORM,
		R16G16_FLOAT,
		R32G32_UINT,
		R32G32_FLOAT,

		// 3 channels
		R8G8B8_UNORM,
		R16G16B16_UNORM,
		R16G16B16_FLOAT,
		R32G32B32_UINT,
		R32G32B32_FLOAT,

		// 4 channels
		R8G8B8A8_UNORM,
		B8G8R8A8_UNORM,
		R16G16B16A16_UNORM,
		R16G16B16A16_FLOAT,
		R32G32B32A32_UINT,
		R32G32B32A32_FLOAT,

		// Depth/Stencil
		D16_UNORM,
		X8_D24_UNORM, // 32 bits: 8 bits unused - 24 bits for depth
		D32_FLOAT,
		S8_UINT,
		D16_UNORM_S8_UINT,
		D24_UNORM_S8_UINT,
		D32_UNORM_S8_UINT,

		Count,
		Invalid = Count,
	};

	struct PlatformData
	{
#ifdef FUJI_WIN32
		HINSTANCE hinstance;
		HWND hwnd;
#else // FUJI_WIN32
#error Platform not supported!
#endif // FUJI_WIN32
	};

	struct SwapchainSettings
	{
		uint32_t image_count = MAX_SWAPCHAIN_IMAGE_COUNT;
	};

	struct ContextSettings
	{
		PlatformData platform_data;
		SwapchainSettings swapchain_settings;
		const char* app_name = "fuji_app";
		uint32_t width;
		uint32_t height;
		bool enable_validation = false;
	};

	struct Backend; // Implementation specific. Needs to be defined in the implementation translation unit.

	struct Context
	{
		ContextSettings settings;
		Backend* backend = nullptr;
	};

	// Generic API
	bool create_context(ContextSettings settings, Context& out_context);
	void destroy_context(Context& context);

	// Resource API

	// Command API

} // namespace fuji
