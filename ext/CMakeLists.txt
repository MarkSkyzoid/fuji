find_package(Vulkan REQUIRED)

add_library(vk-bootstrap STATIC)
add_library(VulkanMemoryAllocator INTERFACE)

target_sources(vk-bootstrap PRIVATE 
    "${CMAKE_CURRENT_SOURCE_DIR}/vk-bootstrap/src/VkBootstrap.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/vk-bootstrap/src/VkBootstrap.cpp"
)

target_include_directories(vk-bootstrap PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/vk-bootstrap/src" )
target_link_libraries(vk-bootstrap PUBLIC Vulkan::Vulkan)

target_include_directories(VulkanMemoryAllocator INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/VulkanMemoryAllocator/src/" )
