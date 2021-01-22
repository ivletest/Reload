cmake_minimum_required(VERSION 3.13 FATAL_ERROR)
project(Reload LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})
include(HelperFunctions)

# Add Corrade as a subproject
add_subdirectory(${CMAKE_SOURCE_DIR}/lib/corrade EXCLUDE_FROM_ALL)

# Add Magnum base as a subproject
set(WITH_SDL2APPLICATION ON CACHE BOOL "" FORCE)
set(WITH_GL ON CACHE BOOL "" FORCE)
set(WITH_ANYIMAGEIMPORTER ON CACHE BOOL "" FORCE)
set(WITH_ANYSCENEIMPORTER ON CACHE BOOL "" FORCE)
set(WITH_OBJIMPORTER ON CACHE BOOL "" FORCE)
add_subdirectory(${CMAKE_SOURCE_DIR}/lib/magnum EXCLUDE_FROM_ALL)

# Add Magnum plugins as a subproject
set(WITH_STBIMAGEIMPORTER ON CACHE BOOL "" FORCE)
set(WITH_TINYGLTFIMPORTER ON CACHE BOOL "" FORCE)
set(WITH_ASSIMPIMPORTER ON CACHE BOOL "" FORCE)
add_subdirectory(${CMAKE_SOURCE_DIR}/lib/magnum-plugins EXCLUDE_FROM_ALL)

# Add Magnum extras as a subproject
set(WITH_UI ON CACHE BOOL "" FORCE)
add_subdirectory(${CMAKE_SOURCE_DIR}/lib/magnum-extras EXCLUDE_FROM_ALL)

# Add Magnum integrations as subproject
set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/lib/imgui)
set(WITH_IMGUI ON CACHE BOOL "" FORCE)
add_subdirectory(${CMAKE_SOURCE_DIR}/lib/magnum-integration EXCLUDE_FROM_ALL)

include(GNUInstallDirs)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR})

# defines targets and sources
include(${CMAKE_SOURCE_DIR}/src/CMakeLists.txt)

# Tests are disabled for now
# enable_testing()
# add_subdirectory(tests)

if( MSVC )
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
	add_definitions( -DUNICODE -D_UNICODE )
endif()

# Protect from in source builds
message("current binary dir is ${CMAKE_CURRENT_BINARY_DIR}")

if(${PROJECT_SOURCE_DIR} STREQUAL ${PROJECT_BINARY_DIR})
    message(FATAL_ERROR "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there.")
endif()

include(CMakeCPack.cmake)