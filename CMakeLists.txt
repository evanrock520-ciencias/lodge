cmake_minimum_required(VERSION 3.24)

project(fire)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)

# Dependencies
FetchContent_Declare(
        eigen
        GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
        GIT_TAG 3.4.0
)

set(RAYLIB_VERSION 5.5)
find_package(raylib ${RAYLIB_VERSION} QUIET)
if (NOT raylib_FOUND)
    FetchContent_Declare(
            raylib
            DOWNLOAD_EXTRACT_TIMESTAMP OFF
            URL https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz
    )
endif ()

FetchContent_MakeAvailable(eigen)
if (NOT raylib_FOUND)
    FetchContent_MakeAvailable(raylib)
endif ()

add_subdirectory(lodge)

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} lodge)