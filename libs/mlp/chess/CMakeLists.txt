cmake_minimum_required(VERSION 3.27)
project(mlp_chess_lib VERSION 1.0 LANGUAGES CXX)

add_library(${PROJECT_NAME} STATIC
    board.cpp
    board.hpp
    pgn_parser.cpp
    pgn_parser.hpp
    pgn_playermove.cpp
    pgn_playermove.hpp
    piece.hpp
    square.cpp
    square.hpp
        utility.hpp
)

target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../..")

install(TARGETS ${PROJECT_NAME} DESTINATION lib)
#(FILES ${PROJECT_NAME}_headers DESTINATION include)
