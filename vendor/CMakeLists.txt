add_subdirectory(SDL)
add_subdirectory(imgui)
add_subdirectory(glm)
add_subdirectory(spdlog)
add_subdirectory(cgltf)
add_subdirectory(stb_image)

set(JSON_BuildTests OFF CACHE BOOL "" FORCE)
set(JSON_Install OFF CACHE BOOL "" FORCE)
add_subdirectory(nlohmann_json)

find_package(Vulkan REQUIRED) # 添加VulkanSDK

add_library(vendor INTERFACE)

target_link_libraries(vendor INTERFACE 
    Vulkan::Vulkan
    SDL3::SDL3 
    imgui 
    glm::glm 
    spdlog::spdlog 
    cgltf 
    stb_image 
    nlohmann_json::nlohmann_json 
    entt
)

# entt (header-only)
add_library(entt INTERFACE)
target_include_directories(entt INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/entt/single_include)