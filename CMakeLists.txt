cmake_minimum_required(VERSION 3.20)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)

project(ShadowAssassin VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Configure CMake search paths for custom/local user environment (e.g. Linux/WSL dev)
if(UNIX AND NOT APPLE)
    if(EXISTS "$ENV{HOME}/.local")
        list(PREPEND CMAKE_PREFIX_PATH "$ENV{HOME}/.local")
        list(PREPEND CMAKE_INCLUDE_PATH "$ENV{HOME}/.local/include")
        list(PREPEND CMAKE_LIBRARY_PATH "$ENV{HOME}/.local/lib")
        link_directories("$ENV{HOME}/.local/lib")
        include_directories("$ENV{HOME}/.local/include")
    endif()
endif()

# Raylib Integration: Local vendored source prioritized, FetchContent as fallback
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES OFF CACHE BOOL "" FORCE)

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/external/raylib/CMakeLists.txt")
    add_subdirectory(external/raylib)
else()
    include(FetchContent)
    FetchContent_Declare(
        raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG 5.5
        GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(raylib)
endif()

# Include project headers
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Project Source Files (Character Art & Animation Upgrade)
set(PROJECT_SOURCES
    src/main.cpp
    src/GameManager.cpp
    src/Entity.cpp
    src/Player.cpp
    src/Enemy.cpp
    src/Guard.cpp
    src/Obstacle.cpp
    src/Collectible.cpp
    src/CollisionManager.cpp
    src/Level.cpp
    src/UIManager.cpp
    src/CharacterRenderer.cpp
)

add_executable(ShadowAssassin ${PROJECT_SOURCES})

target_link_libraries(ShadowAssassin PRIVATE raylib)

# Compiler flags for clean OOP and code quality
if(MSVC)
    target_compile_options(ShadowAssassin PRIVATE /W4 /permissive-)
else()
    target_compile_options(ShadowAssassin PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Copy assets folder to output directory after build
add_custom_command(TARGET ShadowAssassin POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/assets
        $<TARGET_FILE_DIR:ShadowAssassin>/assets
)

# Phase Test Executables for Academic Grading and CI/CD Verification
option(BUILD_TESTS "Build Phase test executables" ON)
if(BUILD_TESTS)
    set(CORE_SOURCES
        src/Entity.cpp
        src/Player.cpp
        src/Enemy.cpp
        src/Guard.cpp
        src/Obstacle.cpp
        src/Collectible.cpp
        src/CollisionManager.cpp
        src/CharacterRenderer.cpp
    )

    add_executable(test_phase2 tests/test_phase2.cpp ${CORE_SOURCES})
    target_link_libraries(test_phase2 PRIVATE raylib)

    add_executable(test_phase3 tests/test_phase3.cpp ${CORE_SOURCES})
    target_link_libraries(test_phase3 PRIVATE raylib)

    add_executable(test_phase4 tests/test_phase4.cpp ${CORE_SOURCES})
    target_link_libraries(test_phase4 PRIVATE raylib)

    add_executable(test_phase5 tests/test_phase5.cpp ${CORE_SOURCES} src/Level.cpp src/GameManager.cpp src/UIManager.cpp)
    target_link_libraries(test_phase5 PRIVATE raylib)

    add_executable(test_phase6 tests/test_phase6.cpp ${CORE_SOURCES} src/Level.cpp src/UIManager.cpp src/GameManager.cpp)
    target_link_libraries(test_phase6 PRIVATE raylib)

    add_executable(test_phase7 tests/test_phase7_integration.cpp ${CORE_SOURCES} src/Level.cpp src/UIManager.cpp src/GameManager.cpp)
    target_link_libraries(test_phase7 PRIVATE raylib)
endif()
