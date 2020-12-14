#if FUJI_API == FUJI_API_VULKAN
#include "../fuji_private.hpp"

#ifdef FUJI_WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#else
#error Platform not supported!
#endif

#include "../../../ext/vk-bootstrap/src/VkBootstrap.h"
#include "../../../ext/VulkanMemoryAllocator/src/vk_mem_alloc.h"

#include <vector>

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

	struct SwapchainVK
	{
		VkSwapchainKHR swapchain;
		VkFormat format;
		std::vector<VkImage> images;
		std::vector<VkImageView> image_views;
	};

	struct QueueVK
	{
		uint32_t queue_family_index = -1;
		VkQueue queue;
	};

	struct Backend
	{
		const API api = API::Vulkan;

		VkInstance instance;
		VkPhysicalDevice physical_device;
		VkDevice device;
		VkSurfaceKHR surface;

		SwapchainVK swapchain;

		QueueVK queues[to_underlying_type(QueueType::Count)];

		// #TODO: Make it compulsory to have validation enabled only if a macro is defined
		VkDebugUtilsMessengerEXT debug_messenger; // Only present if validation is enabled
	};

	// Utilities
	VkFormat to_vk_format(Format format)
	{
		assert(format != Format::Invalid);

		VkFormat result = VK_FORMAT_UNDEFINED;
		switch (format) {
		// 1 channel
		case Format::R8_UNORM:
			result = VK_FORMAT_R8_UNORM;
			break;
		case Format::R16_UNORM:
			result = VK_FORMAT_R16_UNORM;
			break;
		case Format::R16_FLOAT:
			result = VK_FORMAT_R16_SFLOAT;
			break;
		case Format::R32_UINT:
			result = VK_FORMAT_R32_UINT;
			break;
		case Format::R32_FLOAT:
			result = VK_FORMAT_R32_SFLOAT;
			break;

		// 2 channels
		case Format::R8G8_UNORM:
			result = VK_FORMAT_R8G8_UNORM;
			break;
		case Format::R16G16_UNORM:
			result = VK_FORMAT_R16G16_UNORM;
			break;
		case Format::R16G16_FLOAT:
			result = VK_FORMAT_R16G16_SFLOAT;
			break;
		case Format::R32G32_UINT:
			result = VK_FORMAT_R32G32_UINT;
			break;
		case Format::R32G32_FLOAT:
			result = VK_FORMAT_R32G32_SFLOAT;
			break;

		// 3 channels
		case Format::R8G8B8_UNORM:
			result = VK_FORMAT_R8G8B8_UNORM;
			break;
		case Format::R16G16B16_UNORM:
			result = VK_FORMAT_R16G16B16_UNORM;
			break;
		case Format::R16G16B16_FLOAT:
			result = VK_FORMAT_R16G16B16_SFLOAT;
			break;
		case Format::R32G32B32_UINT:
			result = VK_FORMAT_R32G32B32_UINT;
			break;
		case Format::R32G32B32_FLOAT:
			result = VK_FORMAT_R32G32B32_SFLOAT;
			break;

		// 4 channels
		case Format::B8G8R8A8_UNORM:
			result = VK_FORMAT_B8G8R8A8_UNORM;
			break;
		case Format::R8G8B8A8_UNORM:
			result = VK_FORMAT_R8G8B8A8_UNORM;
			break;
		case Format::R16G16B16A16_UNORM:
			result = VK_FORMAT_R16G16B16A16_UNORM;
			break;
		case Format::R16G16B16A16_FLOAT:
			result = VK_FORMAT_R16G16B16A16_SFLOAT;
			break;
		case Format::R32G32B32A32_UINT:
			result = VK_FORMAT_R32G32B32A32_UINT;
			break;
		case Format::R32G32B32A32_FLOAT:
			result = VK_FORMAT_R32G32B32A32_SFLOAT;
			break;

		// Depth/stencil
		case Format::D16_UNORM:
			result = VK_FORMAT_D16_UNORM;
			break;
		case Format::X8_D24_UNORM:
			result = VK_FORMAT_X8_D24_UNORM_PACK32;
			break;
		case Format::D32_FLOAT:
			result = VK_FORMAT_D32_SFLOAT;
			break;
		case Format::S8_UINT:
			result = VK_FORMAT_S8_UINT;
			break;
		case Format::D16_UNORM_S8_UINT:
			result = VK_FORMAT_D16_UNORM_S8_UINT;
			break;
		case Format::D24_UNORM_S8_UINT:
			result = VK_FORMAT_D24_UNORM_S8_UINT;
			break;
		case Format::D32_FLOAT_S8_UINT:
			result = VK_FORMAT_D32_SFLOAT_S8_UINT;
			break;
		}
		return result;
	}

	bool create_swapchain(const Backend& backend,
								 const ContextSettings& context_settings,
								 SwapchainSettings& in_out_swapchain_settings,
								 SwapchainVK& out_swapchain)
	{
		out_swapchain = {};

		vkb::SwapchainBuilder swapchain_builder_vkb { backend.physical_device, backend.device, backend.surface };

		if (in_out_swapchain_settings.color_format != Format::Undefined) {
			const VkSurfaceFormatKHR desired_format = VkSurfaceFormatKHR {
				to_vk_format(in_out_swapchain_settings.color_format),
				VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,
			};

			swapchain_builder_vkb.set_desired_format(desired_format);
		} else {
			swapchain_builder_vkb.use_default_format_selection();
		}

		auto maybe_swapchain_vkb =
		swapchain_builder_vkb.set_desired_extent(context_settings.width, context_settings.height)
		.set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
		.build();
		if (!maybe_swapchain_vkb.has_value()) {
			return false;
		}

		vkb::Swapchain swapchain_vkb = maybe_swapchain_vkb.value();
		if (!swapchain_vkb.get_images().has_value()) {
			return false;
		}
		if (!swapchain_vkb.get_image_views().has_value()) {
			return false;
		}

		in_out_swapchain_settings.image_count = swapchain_vkb.image_count;

		// Write object out
		out_swapchain.format = swapchain_vkb.image_format;
		out_swapchain.swapchain = swapchain_vkb.swapchain;
		out_swapchain.images = swapchain_vkb.get_images().value();
		out_swapchain.image_views = swapchain_vkb.get_image_views().value();

		return true;
	}

	void destroy_swapchain(Context& context)
	{
		const Backend& backend = *context.backend;
		const SwapchainSettings& swapchain_settings = context.settings.swapchain_settings;

		vkDestroySwapchainKHR(backend.device, backend.swapchain.swapchain, nullptr);

		for (uint32_t i = 0; i < swapchain_settings.image_count; i++) {
			vkDestroyImageView(backend.device, backend.swapchain.image_views[i], nullptr);
		}
	}

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

		// Write final settings into the context
		out_context.settings = settings;

		// init instance
		vkb::InstanceBuilder instance_builder_vkb;

		auto maybe_instance_vkb = instance_builder_vkb.set_app_name(out_context.settings.app_name)
										  .request_validation_layers(out_context.settings.enable_validation)
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
		surface_create_info.hinstance = out_context.settings.platform_data.hinstance;
		surface_create_info.hwnd = out_context.settings.platform_data.hwnd;
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

		// Queues
		backend.queues[to_underlying_type(QueueType::Graphics)].queue =
		device_vkb.get_queue(vkb::QueueType::graphics).value();
		backend.queues[to_underlying_type(QueueType::Graphics)].queue_family_index =
		device_vkb.get_queue_index(vkb::QueueType::graphics).value();

		backend.queues[to_underlying_type(QueueType::Present)].queue =
		device_vkb.get_queue(vkb::QueueType::present).value();
		backend.queues[to_underlying_type(QueueType::Present)].queue_family_index =
		device_vkb.get_queue_index(vkb::QueueType::present).value();

		backend.queues[to_underlying_type(QueueType::Transfer)].queue =
		device_vkb.get_queue(vkb::QueueType::transfer).value();
		backend.queues[to_underlying_type(QueueType::Transfer)].queue_family_index =
		device_vkb.get_queue_index(vkb::QueueType::transfer).value();

		backend.queues[to_underlying_type(QueueType::Compute)].queue =
		device_vkb.get_queue(vkb::QueueType::compute).value();
		backend.queues[to_underlying_type(QueueType::Compute)].queue_family_index =
		device_vkb.get_queue_index(vkb::QueueType::compute).value();

		// Swapchain
		if (!create_swapchain(backend, out_context.settings, out_context.settings.swapchain_settings,
									 backend.swapchain)) {
			return false;
		}

		return true;
	}

	void destroy_context(Context& context)
	{
		assert(context.backend != nullptr);

		Backend& backend = *context.backend;
		assert(backend.api == API::Vulkan);

		// Order is important (opposite of creation order)
		destroy_swapchain(context);

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