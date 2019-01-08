cmake_minimum_required(VERSION 3.0)

project(CthulhuVsSatan
    VERSION 0.1.0
)

set(CMAKE_CXX_FLAGS "-Wall -Wextra -std=c++14 -O0 -g")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(LATEX)

if(LATEX_FOUND)
    add_subdirectory(docs EXCLUDE_FROM_ALL)
endif(LATEX_FOUND)

find_package(Threads REQUIRED)
find_package(gf REQUIRED)
find_package(Boost REQUIRED)

add_custom_target(check
    COMMAND clang-format -style=file -i src/* include/*
    COMMAND run-clang-tidy -p "${CMAKE_BINARY_DIR}" -header-filter=.* -checks=cppcoreguidelines-*,bugprone-*,misc-*,performance-*,portability-*,readability-*
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

add_executable(game
    src/action.cpp
    src/character.cpp
    src/entitycharacter.cpp
    src/game.cpp
    src/gameai.cpp
    src/goal.cpp
    src/main.cpp
    src/player.cpp
    src/humanplayer.cpp
    src/utility.cpp
)

target_include_directories(game PRIVATE
	${Boost_INCLUDE_DIRS}
	include
)

target_link_libraries(game
	Threads::Threads
    gf::gf0
)
