add_executable(ExplicitClient_Lab4 ExplicitClient_Lab4.cpp)

target_compile_features(ExplicitClient_Lab4 PRIVATE cxx_std_20)

add_custom_command(TARGET ExplicitClient_Lab4 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${CMAKE_CURRENT_SOURCE_DIR}/../FileIntegrityDynamic.dll"
    "$<TARGET_FILE_DIR:ExplicitClient_Lab4>"
)