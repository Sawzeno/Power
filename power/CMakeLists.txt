cmake_minimum_required(VERSION 3.10)
project(power)

add_compile_definitions(PROJECT_DIR="${CMAKE_SOURCE_DIR}")

MESSAGE(STATUS "Compiler being used: ${CMAKE_C_COMPILER}")

MESSAGE(STATUS "LINKING SRC")

set(SOURCES_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
file(GLOB SOURCES "${SOURCES_DIR}/*.cpp")

add_executable(main ${SOURCES})


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

    target_include_directories(main
    PRIVATE
    ${GLEW_INCLUDE_DIR}
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

# for linux

if(UNIX AND NOT APPLE)
	#find glew library and headers
	find_package(GLEW REQUIRED)

	MESSAGE(STATUS "LINKING GLEW --LINUX")

	#link the main target with GLEW library on linux
	target_link_libraries(main
		PRIVATE
		GLEW::GLEW
	)

	#Include GLEW Headers for the main
	target_include_directories(main
		PUBLIC
		${GLEW_INCLUDE_DIRS}
	)
endif()
