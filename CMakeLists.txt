cmake_minimum_required(VERSION 3.27)
project(chess VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)

add_subdirectory(libs/mlp/chess)

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} mlp_chess_lib)
