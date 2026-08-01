add_library(spirv_reflect STATIC
    spirv_reflect.c
)

target_include_directories(spirv_reflect PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(spirv_reflect PUBLIC c_std_11)
