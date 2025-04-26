cmake_minimum_required(VERSION 3.18)
project(cs416_prog CXX)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
cmake_policy(SET CMP0074 NEW)

set(default_build_type "Release")
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
  set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
    "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Specific C++ features

# Not all versions of Clang commonly used have this feature of C++17, even if they support C++17 generally
# so we need to check
include(CheckCXXSourceCompiles)
CHECK_CXX_SOURCE_COMPILES(
  "#include <new>
  int main() { float* values = new (std::align_val_t(32)) float[100]; return 0; }"
  HAVE_ALIGN_VAL
)
if (HAVE_ALIGN_VAL)
  add_compile_definitions(HAVE_ALIGN_VAL)
endif()

# Check if X86 intrinsics are available
include(CheckIncludeFileCXX)
CHECK_INCLUDE_FILE_CXX("immintrin.h" HAVE_INTRINSICS)

set(CMAKE_CXX_FLAGS "-m64 -Wall")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
# g++ on Ubuntu (used by Gradescope) is configured with position independent executable flag which
# causes link-time errors. Disable only in that setting.
set(CMAKE_CXX_FLAGS_GRADESCOPE "-no-pie -O3"
  CACHE
  STRING "Flags used by the C++ compiler during Gradescope builds."
  FORCE
)
mark_as_advanced(CMAKE_CXX_FLAGS_GRADESCOPE)

OPTION(DEFINE_VERBOSE
  "Build the project using verbose code"
  OFF)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# ISPC Support
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/AddISPCObject.cmake)
find_program(ISPC_EXECUTABLE ispc DOC "Path to the ISPC executable.")
if (NOT ISPC_EXECUTABLE)
    message(STATUS "Could not find Intel SPMD Compiler (ispc). Make sure you have completed the 'Getting Started' instructions on Canvas.")
else()
    message(STATUS "Found Intel SPMD Compiler (ispc): ${ISPC_EXECUTABLE}")
endif()

# Specify arguments (as a list)
set(ISPC_FLAGS "-O3;--math-lib=default")

# CUDA Support
set(CMAKE_CUDA_STANDARD 14)
set(CMAKE_CUDA_STANDARD_REQUIRED ON)
include(CheckLanguage)
check_language(CUDA)
if(CMAKE_CUDA_COMPILER)
  enable_language(CUDA)
else()
  message(STATUS "Could not find the CUDA compiler and toolkit. If you have CUDA-compatible GPU install the CUDA compiler/toolkit.")
endif()

# OpenMP Support
if (CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
  find_package(OSXOpenMP)
else()
  find_package(OpenMP)
endif()
if(NOT OpenMP_FOUND)
  message(STATUS "Could not find compiler with OpenMP support. Make sure you have completed the 'Getting Started' instructions on Canvas.")
endif()

include_directories(BEFORE ${CMAKE_CURRENT_SOURCE_DIR}/common/include)

add_subdirectory(common)
if(IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/pa1" AND ISPC_EXECUTABLE)
  # We will strip out other assignments on Gradescope
  add_subdirectory(pa1)
endif()
if(IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/pa2")
  # We will strip out other assignments on Gradescope
  add_subdirectory(pa2)
endif()
if (IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/pa3" AND CMAKE_CUDA_COMPILER)
  # We need a CUDA compiler and we will strip out other assignments on Gradescope
  add_subdirectory(pa3)
endif()
if(IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/pa4" AND OpenMP_FOUND)
  # We will strip out other assignments on Gradescope
  add_subdirectory(pa4)
endif()

if(IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/fp")
  # We will strip out other assignments on Gradescope
  add_subdirectory(fp)
endif()

# Build in rsync command for copying code
get_filename_component(ADA_DIR ${CMAKE_SOURCE_DIR} NAME CACHE)
set(ADA_USER "$ENV{USER}" CACHE STRING "Middlebury username")
add_custom_target(
    ada-copy
    COMMAND "rsync" "-avzP" --filter=':- .gitignore' "." "${ADA_USER}@ada.middlebury.edu:${ADA_DIR}"
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    COMMENT "Copy files to ada"
)

