cmake_minimum_required(VERSION 3.16)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/run/")
project(lab7 CXX)


find_package(glog 0.7.1 REQUIRED)
file(GLOB_RECURSE utils_src "${CMAKE_SOURCE_DIR}/src/utils/**.cpp")
file(GLOB_RECURSE utils_heads "${CMAKE_SOURCE_DIR}/src/utils/**.hpp")

add_executable(server
    src/server.cpp ${utils_heads} ${utils_src} 
)
add_executable(client
    src/client.cpp ${utils_heads} ${utils_src}
)

target_link_libraries(server PRIVATE glog::glog)
target_link_libraries(client PRIVATE glog::glog)
target_include_directories(server PRIVATE "${CMAKE_SOURCE_DIR}/src/utils")
target_include_directories(client PRIVATE "${CMAKE_SOURCE_DIR}/src/utils")
