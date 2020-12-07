#if FUJI_API == FUJI_API_VULKAN
#include "../fuji_private.hpp"
#include "../../../ext/vk-bootstrap/src/VkBootstrap.h"
#include "../../../ext/VulkanMemoryAllocator/src/vk_mem_alloc.h"

namespace fuji {
	// Structures
	enum class API
	{
		Vulkan,
	};

	struct Backend
	{
		const API api = API::Vulkan;
		// #TODO: Vulkan objects here!
	};

	// Generic API
	bool create_context(ContextSettings settings, Context& out_context)
	{
		if (!validate_context_settings(settings)) {
			false;
		}

		out_context = {};
		out_context.backend = new Backend {};

		settings.swapchainImageCount =
		std::min(settings.swapchainImageCount, ContextSettings::MAX_SWAPCHAIN_IMAGE_COUNT);

		out_context.settings = settings;

		return true;
	}

	void destroy_context(Context& context) { delete context.backend; }
} // namespace fuji

#endif // FUJI_API