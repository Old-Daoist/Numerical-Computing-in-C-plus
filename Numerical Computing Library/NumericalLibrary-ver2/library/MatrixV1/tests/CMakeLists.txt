include(CTest)

# ─── Catch2 (amalgamated, no system install required) ────────────────────────
add_library(catch2_lib STATIC
    catch2/catch_amalgamated.cpp
)
target_include_directories(catch2_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# ─── matrix_tests executable ─────────────────────────────────────────────────
add_executable(matrix_tests
    test_smoke.cpp
    test_matrix.cpp
)

target_link_libraries(matrix_tests PRIVATE
    matrix_lib
    catch2_lib
)

target_include_directories(matrix_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# ─── Register with CTest ─────────────────────────────────────────────────────
add_test(NAME matrix_tests COMMAND matrix_tests)
