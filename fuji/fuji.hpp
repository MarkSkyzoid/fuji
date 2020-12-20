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

#include <cassert>
#include <cstdint>

// Api implementation here
namespace fuji {

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
		D32_FLOAT_S8_UINT,

		Count,
		Invalid = Count,
	};

	struct ClearValueColor
	{
		float r, g, b, a;
	};

	struct ClearValueDepthStencil
	{
		float depth;
		uint32_t stencil;
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
		// NOTE: This is set internally by fuji::create_context
		// It can later be queried again.
		uint32_t image_count = 0;
		// If this is set to Format::Undefined then pick up a standard format automatically
		Format color_format = Format::Undefined;
		ClearValueColor color_clear_value;
		// #TODO: Depth stencil buffer
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

	// Important objects
	struct Context
	{
		ContextSettings settings;
		Backend* backend = nullptr;
	};

	enum class QueueType : uint8_t
	{
		Graphics = 0,
		Present,
		Transfer,
		Compute,

		Count,
		Invalid = Count,
	};

	// Resource Handles
	enum class ResourceType : uint8_t
	{
		CommandPool = 0,
		Command,

		Count,
		Invalid = Count
	};

	// Handles are invalid when created
	template<ResourceType Tag> struct Handle
	{
		static constexpr uint64_t INVALID_VALUE = static_cast<uint64_t>(-1);
		uint64_t value = INVALID_VALUE;

		bool is_valid() const { return value != INVALID_VALUE; }
		void make_invalid() { value = INVALID_VALUE; }
	};

	using CommandPoolHandle = Handle<ResourceType::CommandPool>;
	using CommandHandle = Handle<ResourceType::Command>;

	// Generic API
	bool create_context(ContextSettings settings, Context& out_context);
	void destroy_context(Context& context);

	// Resource API
	CommandPoolHandle create_command_pool(Context& context, QueueType queue_type, bool transient);
	void destroy_command_pool(Context& context, CommandPoolHandle& command_pool_handle);

	// Command API

} // namespace fuji
