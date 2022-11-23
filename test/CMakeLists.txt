set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mrdrnd")

add_executable(seed seed.cpp)
add_executable(gen gen.cpp)