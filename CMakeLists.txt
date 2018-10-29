cmake_minimum_required(VERSION 3.0)

project(CthulhuVsSatan
    VERSION 0.1.0
)

set(CMAKE_CXX_FLAGS "-Wall -Wextra -std=c++14 -O0 -g")

find_package(LATEX)

if(LATEX_FOUND)
    add_subdirectory(docs EXCLUDE_FROM_ALL)
endif(LATEX_FOUND)

find_package(gf REQUIRED)

add_executable(game
    src/main.cpp
    src/game.cpp
)

target_include_directories(game PRIVATE include)

target_link_libraries(game
    gf::gf0
)
