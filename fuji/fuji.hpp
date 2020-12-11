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
	// Structures
	struct PlatformData
	{
#ifdef FUJI_WIN32
		HINSTANCE hinstance;
		HWND hwnd;
#else // FUJI_WIN32
#error Platform not supported!
#endif // FUJI_WIN32
	};

	struct ContextSettings
	{
		static constexpr uint32_t MAX_SWAPCHAIN_IMAGE_COUNT = 3;
		
		PlatformData platform_data;
		const char* app_name = "fuji_app";
		uint32_t width;
		uint32_t height;
		uint32_t swapchain_image_count = MAX_SWAPCHAIN_IMAGE_COUNT;
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
