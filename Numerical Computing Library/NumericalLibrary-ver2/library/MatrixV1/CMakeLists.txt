cmake_minimum_required(VERSION 3.15)
project(Matrix VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ─── Compiler warnings ────────────────────────────────────────────────────────
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# ─── matrix_lib (static library — all algorithm sources) ─────────────────────
add_library(matrix_lib STATIC
    src/Matrix.cpp

    src/operations/Add.cpp
    src/operations/Subtract.cpp
    src/operations/Multiply.cpp
    src/operations/Divide.cpp

    src/solvers/GaussElimination.cpp
    src/solvers/GaussJacobi.cpp
    src/solvers/GaussSeidel.cpp
    src/solvers/LUDecomposition.cpp
    src/solvers/Cholesky.cpp
    src/solvers/QR.cpp

    src/numerical/Eigen.cpp
    src/numerical/EigenPower.cpp
    src/numerical/Interpolation.cpp
)

target_include_directories(matrix_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# ─── matrix_app (console executable) ─────────────────────────────────────────
add_executable(matrix_app main.cpp)

target_link_libraries(matrix_app PRIVATE matrix_lib)

# ─── Tests (Catch2) ───────────────────────────────────────────────────────────
enable_testing()
add_subdirectory(tests)
