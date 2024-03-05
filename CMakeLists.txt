cmake_minimum_required(VERSION 3.0)

project(noch)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

SET(NOCH_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}/includes
)

SET(NOCH_SOURCES
    sources/detail/imurmur3.cpp
)

find_package(Boost REQUIRED)

add_library(noch-lib SHARED ${NOCH_SOURCES})
target_include_directories(noch-lib PUBLIC ${NOCH_INCLUDE_DIRS})
target_link_libraries(noch-lib PRIVATE ${Boost_LIBRARIES})

install(DIRECTORY ${NOCH_INCLUDE_DIRS} DESTINATION include)

add_subdirectory(examples)

