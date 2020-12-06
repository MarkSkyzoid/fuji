project(fuji_main)

file(GLOB fuji_SRC
    "*.hpp"
    "*.cpp"
    "vulkan/*.hpp"
    "vulkan/*.cpp"
)

add_library(fuji ${fuji_SRC})

set(GraphicsAPIs
    Vulkan
    Direct3D12
)
set(GraphicsAPI Vulkan CACHE STRING "Vulkan")
set_property(CACHE GraphicsAPI PROPERTY STRINGS ${GraphicsAPIs})
list(FIND GraphicsAPIs ${GraphicsAPI} index)
if(index EQUAL 0) # Vulkan
    add_compile_definitions(FUJI_API=FUJI_API_VULKAN)
    find_package(Vulkan REQUIRED)
    target_link_libraries(fuji vk-bootstrap VulkanMemoryAllocator)
elseif(index EQUAL 1)
    add_compile_definitions(FUJI_API=FUJI_API_DX12)
else()
    message(FATAL_ERROR "GraphicsAPI must be one of ${GraphicsAPIs}")
endif()

