#if FUJI_API == FUJI_API_VULKAN
#include "../fuji.hpp"
#include "../../ext/vk-bootstrap/src/VkBootstrap.h"
#include "../../ext/VulkanMemoryAllocator/src/vk_mem_alloc.h"

namespace fuji {
	// Structures
	struct Backend
	{
		// #TODO: Vulkan objects here!
	};

	// Generic API

	fuji::Context create_context(ContextSettings settings)
	{
		Context out_context = Context { settings, Backend {} };

		settings.swapchainImageCount =
		std::min(settings.swapchainImageCount, ContextSettings::MAX_SWAPCHAIN_IMAGE_COUNT);

		out_context.settings = settings;

		return out_context;
	}

	void destroy_context(Context& /*context*/) {}
} // namespace fuji

#endif // FUJI_API