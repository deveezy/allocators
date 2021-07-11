cmake_minimum_required(VERSION 3.0.0)

set(proj allocator)
project(${proj} CXX)

set(CMAKE_CXX_STANDARD 17)

file(GLOB TARGET_SRC
	"./src/*.cpp", "./src/*.hpp")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_GNU_SOURCE")

add_executable(${proj} ${TARGET_SRC})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
