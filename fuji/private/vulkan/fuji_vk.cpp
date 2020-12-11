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

		// #TODO: Make it compulsory to have validation enabled only if a macro is defined
		VkDebugUtilsMessengerEXT debug_messenger; // Only present if validation is enabled
	};

	// Generic API
	bool create_context(ContextSettings settings, Context& out_context)
	{
		if (!validate_context_settings(settings)) {
			false;
		}

		if (out_context.backend != nullptr) {
			return false;
		}

		out_context = {};
		out_context.backend = new Backend {};
		Backend& backend = *out_context.backend;

		settings.swapchain_image_count =
		std::min(settings.swapchain_image_count, ContextSettings::MAX_SWAPCHAIN_IMAGE_COUNT);

		// Write final settings into the context
		out_context.settings = settings;

		// init instance
		vkb::InstanceBuilder instance_builder_vkb;

		auto maybe_instance_vkb = instance_builder_vkb.set_app_name(settings.app_name)
										  .request_validation_layers(settings.enable_validation)
										  .require_api_version(1, 1, 0)
										  .use_default_debug_messenger()
										  .build();

		if (!maybe_instance_vkb.has_value()) {
			return false;
		}

		vkb::Instance instance_vkb = maybe_instance_vkb.value();
		backend.instance = instance_vkb.instance;
		if (out_context.settings.enable_validation) {
			backend.debug_messenger = instance_vkb.debug_messenger;
		}

		// Create surface
		// #TODO: Move into a create_surface function
#ifdef FUJI_WIN32
		VkWin32SurfaceCreateInfoKHR surface_create_info;
		surface_create_info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		surface_create_info.pNext = NULL;
		surface_create_info.flags = 0;
		surface_create_info.hinstance = settings.platform_data.hinstance;
		surface_create_info.hwnd = settings.platform_data.hwnd;
		VkResult surface_create_result =
		vkCreateWin32SurfaceKHR(backend.instance, &surface_create_info, nullptr, &backend.surface);
		assert(VK_SUCCESS == surface_create_result);
#else
#error Platform not supported
#endif // FUJI_WIN32

		// Create Physical Device
		vkb::PhysicalDeviceSelector physical_device_selector_vkb { instance_vkb };

		auto maybe_physical_device_vkb =
		physical_device_selector_vkb.set_minimum_version(1, 1).set_surface(backend.surface).select();
		if (!maybe_physical_device_vkb.has_value()) {
			return false;
		}

		vkb::PhysicalDevice physical_device_vkb = maybe_physical_device_vkb.value();
		backend.physical_device = physical_device_vkb.physical_device;

		// Create Logical Device
		vkb::DeviceBuilder device_builder_vkb { physical_device_vkb };

		auto maybe_device_vkb = device_builder_vkb.build();
		if (!maybe_device_vkb.has_value()) {
			return false;
		}

		vkb::Device device_vkb = maybe_device_vkb.value();
		backend.device = device_vkb.device;

		return true;
	}

	void destroy_context(Context& context)
	{
		assert(context.backend != nullptr);

		Backend& backend = *context.backend;
		assert(backend.api == API::Vulkan);

		// Order is important (opposite of creation order)
		vkDestroyDevice(backend.device, nullptr);
		vkDestroySurfaceKHR(backend.instance, backend.surface, nullptr);
		if (context.settings.enable_validation) {
			vkb::destroy_debug_utils_messenger(backend.instance, backend.debug_messenger);
		}
		vkDestroyInstance(backend.instance, nullptr);

		// Delete backend and reset to nullptr
		delete context.backend;
		context.backend = nullptr;
	}
} // namespace fuji

#endif // FUJI_API