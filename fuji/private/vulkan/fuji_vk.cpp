#if FUJI_API == FUJI_API_VULKAN
#include "../fuji_private.hpp"

#ifdef FUJI_WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#else
#error Platform not supported!
#endif

#include "../../../ext/vk-bootstrap/src/VkBootstrap.h"
#include "../../../ext/VulkanMemoryAllocator/src/vk_mem_alloc.h"

#include <cassert>

#define VK_CHECK(x)     \
	do {                 \
		VkResult err = x; \
		if (err) {        \
			assert();      \
		}                 \
	} while (0)

namespace fuji {
	// Structures
	enum class API
	{
		Vulkan,
	};

	struct Backend
	{
		const API api = API::Vulkan;

		VkInstance instance;
		VkPhysicalDevice physical_device;
		VkDevice device;
		VkSurfaceKHR surface;
	};

	// Generic API
	bool create_context(ContextSettings settings, Context& out_context)
	{
		if (!validate_context_settings(settings)) {
			false;
		}

		out_context = {};
		out_context.backend = new Backend {};
		Backend& backend = *out_context.backend;

		settings.swapchain_image_count =
		std::min(settings.swapchain_image_count, ContextSettings::MAX_SWAPCHAIN_IMAGE_COUNT);

		// init instance
		vkb::InstanceBuilder builder;
		auto maybe_instance = builder.set_app_name(settings.app_name)
									 .request_validation_layers(settings.enable_validation)
									 .require_api_version(1, 1, 0)
									 .use_default_debug_messenger()
									 .build();

		if (!maybe_instance.has_value()) {
			return false;
		}

		vkb::Instance vkb_instance = maybe_instance.value();
		backend.instance = vkb_instance.instance;

		// Create surface
		// #TODO: Move into a create_surface function
#ifdef FUJI_WIN32
		VkWin32SurfaceCreateInfoKHR surface_create_info;
		surface_create_info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		surface_create_info.pNext = NULL;
		surface_create_info.flags = 0;
		surface_create_info.hinstance = settings.platform_data.hinstance;
		surface_create_info.hwnd = settings.platform_data.hwnd;
		VkResult surface_create_result = vkCreateWin32SurfaceKHR(backend.instance, &surface_create_info, nullptr, &backend.surface);
		assert(VK_SUCCESS == surface_create_result);
#else
#error Platform not supported
#endif // FUJI_WIN32

		out_context.settings = settings;

		return true;
	}

	void destroy_context(Context& context) { delete context.backend; }
} // namespace fuji

#endif // FUJI_API