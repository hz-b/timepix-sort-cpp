cmake_minimum_required(VERSION 3.0.0)

project("timepix-sort-c++" LANGUAGES CXX)

set(TIMEPIX_SORT_CPP_VERSION 0.1.0)

OPTION (USE_OpenMP "Use OpenMP" ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE RELEASE) # default to release build
  message(STATUS "Default to Release build")
endif()

if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
  set(timepix_sort_cpp_DEBUG_BUILD ON)
else()
  set(timepix_sort_cpp_DEBUG_BUILD OFF)
endif()

IF(USE_OpenMP)
    FIND_PACKAGE(OpenMP)
    IF(OPENMP_FOUND)
        message(STATUS "Open MP enabled using flags CFLAGS=${OpenMP_C_FLAGS} CXXFLAGS=${OpenMP_CXX_FLAGS}")
        SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS} ")
        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS} ")
    ENDIF()
ENDIF()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

enable_testing()

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

add_subdirectory(src)
set( ../external-repositories/mmap_allocator/)
add_subdirectory(mmap_allocator_build)
add_subdirectory(tests)
