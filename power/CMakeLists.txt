cmake_minimum_required(VERSION 3.10)
project(power)

set(CMAKE_PREFIX_PATH "D:/Power/vendor/glew")
set(CMAKE_LIBRARY_PATH "D:/Power/vendor/glew/lib/Release/x64")
find_path(GLEW_INCLUDE_DIR GL/glew.h)
find_library(GLEW_LIBRARY NAMES GLEW glew32 glew glew32s PATH_SUFFIXES lib64)
MESSAGE("-----------------------------------------------------")
MESSAGE(${GLEW_INCLUDE_DIR})
MESSAGE(${GLEW_LIBRARY})
add_executable(main main.cpp)

MESSAGE(STATUS "LINKING SRC")

target_link_libraries(main
  PUBLIC
  srclib
)

MESSAGE(STATUS "LINKING GLFW")

target_link_libraries(main
  PRIVATE
  glfw
)

MESSAGE(STATUS "LINKING GLEW")
target_link_libraries(main
  PRIVATE
  ${GLEW_LIBRARY}
)

MESSAGE(STATUS "LINKING OPENGL")

find_package(OpenGL REQUIRED)

target_link_libraries(main
  PRIVATE
  OpenGL::GL
)

target_include_directories(main
PUBLIC
${CMAKE_CURRENT_SOURCE_DIR}/../includes
)

target_include_directories(main
PUBLIC
${GLEW_INCLUDE_DIR}
)

# Copy GLEW DLL to the build directory
add_custom_command(TARGET main POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_PREFIX_PATH}/bin/Release/x64/glew32.dll"
        $<TARGET_FILE_DIR:main>
)