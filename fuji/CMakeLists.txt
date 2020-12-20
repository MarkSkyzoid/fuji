project(fuji)

function(assign_source_group)
    foreach(_source IN ITEMS ${ARGN})
        if (IS_ABSOLUTE "${_source}")
            file(RELATIVE_PATH _source_rel "${CMAKE_CURRENT_SOURCE_DIR}" "${_source}")
        else()
            set(_source_rel "${_source}")
        endif()
        get_filename_component(_source_path "${_source_rel}" PATH)
        string(REPLACE "/" "\\" _source_path_msvc "${_source_path}")
        source_group("${_source_path_msvc}" FILES "${_source}")
    endforeach()
endfunction(assign_source_group)

file(GLOB_RECURSE fuji_own_SRC RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
	"*.hpp"
	"*.cpp"
    "private/*.hpp"
    "private/*.cpp"
    "private/vulkan/*.hpp"
    "private/vulkan/*.cpp"
)

file(GLOB fuji_SRC RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" ${fuji_own_SRC})
assign_source_group(${fuji_SRC})

add_library(fuji STATIC ${fuji_SRC})
set_property(TARGET fuji PROPERTY CXX_STANDARD 17)

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
    target_link_libraries(fuji Vulkan::Vulkan vk-bootstrap VulkanMemoryAllocator)
elseif(index EQUAL 1)
    add_compile_definitions(FUJI_API=FUJI_API_DX12)
else()
    message(FATAL_ERROR "GraphicsAPI must be one of ${GraphicsAPIs}")
endif()

