cmake_minimum_required(VERSION 3.10.2 FATAL_ERROR)

## Use -std=c++17 as default.
set(CMAKE_CXX_STANDARD 17)
## Disable C++ extensions
set(CMAKE_CXX_EXTENSIONS OFF)
## Require full C++ standard
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(hekeyman VERSION 0.1.0)

include(CTest)
enable_testing()

find_package(helib)
find_package(ntl)
link_libraries(helib ntl)

add_library(hekeyman hekeyman.cpp)

add_executable(test.seed test.seed.cpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
