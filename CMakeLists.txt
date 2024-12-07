cmake_minimum_required(VERSION 3.22)

#
# Project settings
#

set(VERSION_MAJOR 0)
set(VERSION_MINOR 0)
set(VERSION_PATCH 0)
set(VERSION_TWEAK 0)

project(
  learnopengl
  VERSION
  "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}.${VERSION_TWEAK}"
)


#
# Helper functions
#

function (assert condition text)
  if(NOT ${condition})
    message(FATAL_ERROR ${text}})
  endif()
endfunction()


#
# System information and configuration
#

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
  set(ARCH "64")
elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
  set(ARCH "32")
endif()

set(CMAKE_DEBUG_POSTFIX _deb)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(CONFIG "deb")
else()
  set(CONFIG "rel")
endif()


#
# Paths
#

# Local paths
set(DIR_ROOT            "${PROJECT_SOURCE_DIR}")
set(DIR_BUILD           "${DIR_ROOT}/build")
set(DIR_DEPENDENCIES    "${DIR_ROOT}/dependencies")
set(DIR_INCLUDE         "${DIR_ROOT}/include")
set(DIR_RESOURCE        "${DIR_ROOT}/resource")
set(DIR_SOURCE          "${DIR_ROOT}/source")
set(DIR_TEST            "${DIR_ROOT}/test")

set(BUILD_TRIPLET       "${CMAKE_SYSTEM_NAME}_${ARCH}_${CONFIG}")
set(BUILD_FOLDER        "${DIR_BUILD}/${CMAKE_SYSTEM_NAME}/${ARCH}/${CONFIG}")
set(DIR_BIN             "${BUILD_FOLDER}/bin")
set(DIR_LIB             "${BUILD_FOLDER}/lib")

# Binary output paths
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY          "${DIR_LIB}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY          "${DIR_LIB}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY          "${DIR_BIN}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG    "${DIR_LIB}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG    "${DIR_LIB}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG    "${DIR_BIN}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE  "${DIR_LIB}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE  "${DIR_LIB}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE  "${DIR_BIN}")


#
# Dependencies
#

find_package(OpenGL REQUIRED COMPONENTS OpenGL)
assert(${OPENGL_FOUND} "OpenGL not found!")

# assimp
option(BUILD_SHARED_LIBS OFF)
add_subdirectory("${DIR_DEPENDENCIES}/assimp")

# glad
add_subdirectory("${DIR_DEPENDENCIES}/glad/cmake" glad_cmake)

# glm
add_subdirectory("${DIR_DEPENDENCIES}/glm")

# imgui
add_subdirectory("${DIR_DEPENDENCIES}/imgui")

# SDL
set(SDL_STATIC TRUE)
add_subdirectory("${DIR_DEPENDENCIES}/SDL")


#
# Compiler
#

set(CMAKE_CXX_STANDARD 20)

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang") # Clang
  #
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU") # GCC
  set(COMPILER_FLAGS "-Wall -Wextra -ansi -pedantic -Wno-format -std=c++17")
  set(COMPILER_FLAGS_DEBUG "-MTd")
  set(COMPILER_FLAGS_RELEASE "-O3 -DNDEBUG -MT")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC") # MSVC
  set(COMPILER_FLAGS "")
  set(COMPILER_FLAGS_DEBUG "/Zi /Ob0 /Od /RTC1 /MTd")
  set(COMPILER_FLAGS_RELEASE "/O2 /Ob2 /DNDEBUG /MT")
endif ()

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${COMPILER_FLAGS} ${COMPILER_FLAGS_DEBUG}")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${COMPILER_FLAGS} ${COMPILER_FLAGS_RELEASE}")

# Macros
add_compile_definitions(
  _CRT_SECURE_NO_WARNINGS
)


# Include paths
include_directories(
  ${DIR_INCLUDE}
  ${DIR_SOURCE}
  ${DIR_DEPENDENCIES}/assimp/include
  ${DIR_DEPENDENCIES}/imgui
  ${DIR_DEPENDENCIES}/imgui/backends
  ${DIR_DEPENDENCIES}/glm
  ${DIR_DEPENDENCIES}/SDL/include
  ${DIR_DEPENDENCIES}/stb
  "glad/include"
)

# Linker paths
link_directories(
  ${DIR_LIB}
)


#
# Files
#

# Resources

set(RESOURCES
  resource/vertex_texture.glsl
  resource/fragment_texture.glsl
)

source_group("resource/" FILES ${RESOURCES})


# Sources

set(SOURCES
)

source_group("source/" FILES ${SOURCES})


#
# Binaries
#

set(EXE ${CMAKE_PROJECT_NAME})
set(LIB ${CMAKE_PROJECT_NAME}_lib)
set(IMGUI ${CMAKE_PROJECT_NAME}_imgui)

#add_library(${LIB} STATIC ${SOURCES})

glad_add_library(glad_gl_core_33 REPRODUCIBLE API gl:core=3.3)

add_executable(${EXE} source/main.cpp ${RESOURCES})
target_link_libraries(${EXE} PRIVATE 
    imgui
    glad_gl_core_33
    SDL3-static
    OpenGL::GL
    assimp-vc143-mt_deb
)

set_target_properties(${EXE} PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
#set_property(TARGET ${EXE} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

