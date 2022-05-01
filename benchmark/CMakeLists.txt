cmake_minimum_required(VERSION 3.16)

include(ExternalProject)
include(FetchContent)

cmake_policy(SET CMP0077 NEW)

SET(
        BASE_BENCHMARK_FILES

        benchmark.cpp
        benchmark.hpp
        fixtures/common_fixture.cpp
        fixtures/common_fixture.hpp
        fixtures/viper_fixture.hpp
)

cmake_host_system_information(RESULT host_name QUERY HOSTNAME)
message(STATUS "Running on host: ${host_name}")
if ("${host_name}" STREQUAL "nvram-01")
    message(STATUS "Known host: nvram01")
    add_definitions(-DNVRAM01)
elseif("${host_name}" STREQUAL "nvram-02")
    message(STATUS "Known host: nvram02")
    add_definitions(-DNVRAM02)
else()
    message(STATUS "Unknown host: ${host_name}")
endif()

# GOOGLE BENCHMARK
FetchContent_Declare(
        google_benchmark
        GIT_REPOSITORY https://github.com/google/benchmark.git
        GIT_TAG v1.5.2
)
set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "Suppressing benchmark's tests" FORCE)
FetchContent_MakeAvailable(google_benchmark)

# PMDK
set(PMDK_INCLUDE_HINTS ${VIPER_PMDK_PATH}/include)
set(PMDK_LIB_HINTS ${VIPER_PMDK_PATH}/lib)

find_path(PMDK_INCLUDE_DIRS libpmem.h HINTS ${PMDK_INCLUDE_HINTS})
find_library(PMDK_LIBRARIES NAMES pmem libpmem HINTS ${PMDK_LIB_HINTS})

set(PMDK_FOUND true)
if (NOT PMDK_INCLUDE_DIRS OR "${PMDK_INCLUDE_DIRS}" STREQUAL "")
    message(FATAL_ERROR "libpmem include directory not found in default locations or custom path (${PMDK_INCLUDE_HINTS}).")
endif ()
if (NOT PMDK_LIBRARIES OR "${PMDK_LIBRARIES}" STREQUAL "")
    message(FATAL_ERROR "libmem not found in default locations or custom path (${PMDK_LIB_HINTS}).")
endif ()

message(STATUS "Found PMDK. Including ${PMDK_INCLUDE_DIRS} and linking ${PMDK_LIBRARIES}")
include_directories(${PMDK_INCLUDE_DIRS})

# libpmemobj + libpmempool
find_library(PMEMOBJ_LIBRARIES NAMES pmemobj libpmemobj HINTS ${PMDK_LIB_HINTS})
find_library(PMEMPOOL_LIBRARIES NAMES pmempool libpmempool HINTS ${PMDK_LIB_HINTS})

# libpmemobj++
# Use -DLIBPMEMOBJ++_PATH=/path/to/libpmemobj++ for custom path
set(LIBPMEMOBJ++_PATH "/scratch/pmem/libpmemobj-cpp" CACHE PATH
        "Path to custom libpmemobj++ install directory")
include_directories(${LIBPMEMOBJ++_PATH}/include)

# TBB
set(TBB_TEST OFF CACHE INTERNAL "")
set(TBB_EXAMPLES OFF CACHE INTERNAL "")
FetchContent_Declare(
        oneTBB
        GIT_REPOSITORY https://github.com/oneapi-src/oneTBB.git
        GIT_TAG        v2021.1.1
)
FetchContent_MakeAvailable(oneTBB)
include_directories(${oneTBB_SOURCE_DIR}/include)



# Use DRAM cache for now
#add_definitions(-DBC_ON_DCPMM=1 -DKVS_ON_DCPMM=1 -DWAL_ON_DCPMM=1)
add_definitions(-DKVS_ON_DCPMM=1 -DWAL_ON_DCPMM=1)

link_directories(${PMEM_ROCKSDB_PREFIX}/src/pmem_rocksdb)
include_directories(${PMEM_ROCKSDB_PREFIX}/src/pmem_rocksdb/include)
include_directories(${PMEM_ROCKSDB_PREFIX}/src/pmem_rocksdb/dcpmm)


# HdrHistogram
set(HDR_HISTOGRAM_BUILD_PROGRAMS OFF CACHE INTERNAL "")
FetchContent_Declare(
        hdr_histogram
        GIT_REPOSITORY https://github.com/HdrHistogram/HdrHistogram_c.git
        GIT_TAG 0.11.2
)
FetchContent_MakeAvailable(hdr_histogram)

set(PMEM_LIBS ${PMDK_LIBRARIES} ${PMEMOBJ_LIBRARIES} ${PMEMPOOL_LIBRARIES})

if (CMAKE_BUILD_TYPE MATCHES Release)
    message(STATUS "BUILDING BENCHMARKS IN RELEASE")
    add_compile_options(-O3 -Ofast -mtune=native)
else()
    message(WARNING "Building benchmarks in ${CMAKE_BUILD_TYPE}. This may impact performance!")
endif()
