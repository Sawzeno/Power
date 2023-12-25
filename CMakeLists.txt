cmake_minimum_required(VERSION 3.10)

project(power)

MESSAGE(STATUS "POWER")
add_subdirectory(power)

MESSAGE(STATUS "INCLUDES")
add_subdirectory(includes)

MESSAGE(STATUS "GLFW")
add_subdirectory(vendor/glfw)


set(GLFW_BUILD_DOCS OFF CACHE BOOL "OFF" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "OFF" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "OFF" FORCE)



