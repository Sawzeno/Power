cmake_minimum_required(VERSION 3.10)
project(power)


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

MESSAGE(STATUS "CONFIGURING GLEW")

# Set the CMake prefix path to the GLEW directory on Windows
if(WIN32)
    set(CMAKE_PREFIX_PATH "D:/Power/vendor/glew")
    set(CMAKE_LIBRARY_PATH "D:/Power/vendor/glew/lib/Release/x64")

    # Find GLEW headers
    find_path(GLEW_INCLUDE_DIR GL/glew.h)

    # Find GLEW library
    find_library(GLEW_LIBRARY NAMES GLEW glew32 glew glew32s PATH_SUFFIXES lib64)

    # Output a message indicating that GLEW is being linked
    MESSAGE(STATUS "LINKING GLEW")

    # Link the main target with GLEW library
    target_link_libraries(main
        PRIVATE
        ${GLEW_LIBRARY}
    )

    # Copy GLEW DLL to the build directory on Windows
    add_custom_command(TARGET main POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_PREFIX_PATH}/bin/Release/x64/glew32.dll"
        $<TARGET_FILE_DIR:main>
    )
endif()

# For macOS
if(APPLE)
    # Find GLEW package
    find_package(GLEW REQUIRED)

    # Output a message indicating that GLEW is being linked on macOS
    MESSAGE(STATUS "LINKING GLEW --MACOS")

    # Link the main target with GLEW library on macOS
    target_link_libraries(main
        PRIVATE
        GLEW::GLEW
    )
endif()

