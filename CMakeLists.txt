cmake_minimum_required(VERSION 3.0)

project(CthulhuVsSatan
    VERSION 0.1.0
)

option(SHOW_BOUNDING_BOXES "Show bounding boxes of sprites" OFF)

# -fsanitize=address -fno-omit-frame-pointer

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(LATEX)

if(LATEX_FOUND)
    add_subdirectory(docs EXCLUDE_FROM_ALL)
endif(LATEX_FOUND)

find_package(Threads REQUIRED)
find_package(gf REQUIRED)

add_custom_target(check
    COMMAND clang-format -style=file -i src/* include/*
    COMMAND run-clang-tidy -p "${CMAKE_BINARY_DIR}" -header-filter=.* -checks=cppcoreguidelines-*,bugprone-*,misc-*,performance-*,portability-*,readability-*
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

add_executable(game
    src/action.cpp
    src/game.cpp
    src/gameai.cpp
    src/main.cpp
    src/utility.cpp
    src/gameboard.cpp
    src/gameboardview.cpp)

target_compile_options(game PRIVATE
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
    -Wall>
    $<$<CXX_COMPILER_ID:MSVC>:
    /W4>)

target_compile_features(game PRIVATE cxx_std_17)

target_include_directories(game PRIVATE
	include
)

target_link_libraries(game
	Threads::Threads
    gf::gf0
)

if (SHOW_BOUNDING_BOXES)
    target_compile_definitions(game PRIVATE
        SHOW_BOUNDING_BOXES
        )
endif (SHOW_BOUNDING_BOXES)
