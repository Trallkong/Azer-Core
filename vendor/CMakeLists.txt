add_subdirectory(glfw)
add_subdirectory(imgui)
add_subdirectory(glm)
add_subdirectory(spdlog)
add_subdirectory(cgltf)
add_subdirectory(stb_image)
add_subdirectory(spirv_reflect)

set(JSON_BuildTests OFF CACHE BOOL "" FORCE)
set(JSON_Install OFF CACHE BOOL "" FORCE)
add_subdirectory(nlohmann_json)

find_package(Vulkan REQUIRED) # 添加VulkanSDK

add_library(vendor INTERFACE)

target_link_libraries(vendor INTERFACE 
    Vulkan::Vulkan
    GLFW
    imgui 
    glm::glm 
    spdlog::spdlog 
    cgltf 
    stb_image 
    nlohmann_json::nlohmann_json
    spirv_reflect
)