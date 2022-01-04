SET(CMAKE_TOOLCHAIN_FILE "C:/Users/Mes/vcpkg/scripts/buildsystems/vcpkg.cmake")
cmake_minimum_required(VERSION 3.21)
project(MesAsio)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(Boost REQUIRED COMPONENTS system)

file(GLOB_RECURSE SOURCE_FILES src/*.cpp)
add_executable(MesAsio ${SOURCE_FILES})
target_include_directories(MesAsio PRIVATE include)
target_link_libraries(MesAsio
                ${Boost_LIBRARIES})
