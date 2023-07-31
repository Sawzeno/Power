cmake_minimum_required(VERSION 3.10)

project(power)

MESSAGE ("MAKING SRCLIB")

add_library(srclib test.cpp)

target_include_directories(srclib
PUBLIC
${CMAKE_CURRENT_SOURCE_DIR}/../includes
)
