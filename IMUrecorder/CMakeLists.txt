cmake_minimum_required(VERSION 2.6)
project(readIMU)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

find_package (Boost REQUIRED COMPONENTS thread system date_time chrono)

add_executable(readIMU readIMU.cpp)

target_include_directories (readIMU SYSTEM PUBLIC
                            ${Boost_INCLUDE_DIRS})

target_link_libraries (readIMU ${Boost_SYSTEM_LIBRARY}
                              ${Boost_THREAD_LIBRARY})

install(TARGETS readIMU RUNTIME DESTINATION bin)
