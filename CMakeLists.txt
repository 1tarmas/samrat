cmake_minimum_required(VERSION 3.12)
project(VCrypt LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenSSL REQUIRED)
find_package(Qt5 COMPONENTS Core Widgets REQUIRED)

add_subdirectory(src)
add_subdirectory(tests)