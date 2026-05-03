add_subdirectory(SDL)
add_subdirectory(imgui)
add_subdirectory(glm)
add_subdirectory(spdlog)

add_library(vendor INTERFACE)
target_link_libraries(vendor INTERFACE SDL3::SDL3 imgui glm::glm spdlog::spdlog)