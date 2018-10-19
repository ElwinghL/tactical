cmake_minimum_required(VERSION 3.0)

project(CthulhuVsSatan
    VERSION 0.1.0
)

add_subdirectory(docs EXCLUDE_FROM_ALL)

find_package(gf REQUIRED)

add_executable(game
    src/main.cpp
)

target_include_directories(game PRIVATE include)

target_link_libraries(game
    gf::gf0
)
