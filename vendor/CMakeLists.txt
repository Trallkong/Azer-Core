add_subdirectory(SDL)
add_subdirectory(imgui)
add_subdirectory(glm)
add_subdirectory(spdlog)
add_subdirectory(cgltf)
add_subdirectory(stb)

set(JSON_BuildTests OFF CACHE BOOL "" FORCE)
set(JSON_Install OFF CACHE BOOL "" FORCE)
add_subdirectory(nlohmann_json)

add_library(vendor INTERFACE)
target_link_libraries(vendor INTERFACE SDL3::SDL3 imgui glm::glm spdlog::spdlog cgltf stb nlohmann_json::nlohmann_json)