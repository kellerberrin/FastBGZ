cmake_minimum_required(VERSION 3.24)
project(KEL_MT_BGZ)

set(CMAKE_CXX_STANDARD 20)

# add extra include directories
include_directories(
        /usr/local/include/spdlog/include/
        kel_utility         # Basic Infrastructure library
        kel_thread          # multi-threading library
        kel_io              # file and network io library
        kel_app             # Application and logging
        )

# zlib for compressed VCF files.
find_package(ZLIB)
if (ZLIB_FOUND)
    include_directories(${ZLIB_INCLUDE_DIRS})
endif()

# Debug g++
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -std=c++20 -fPIC -fconcepts -ggdb")
set(CMAKE_EXE_LINKER_FLAGS_DEBUG "-ggdb")

# Release g++
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -std=c++20 -O3 -s -DNDEBUG")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE " -O3 -s -DNDEBUG")

# Profile g++
set(CMAKE_CXX_FLAGS_PROFILE "-std=c++20 -fPIC -fconcepts -pg -g -ggdb -O3 -Xlinker -Map=output.map ")
set(CMAKE_EXE_LINKER_FLAGS_PROFILE "-pg -g -ggdb -O3 -Xlinker -Map=output.map ")

# All possible warnings
#set(CMAKE_ERROR_WARNING_FLAGS "-std=c++20 -Wall -pthread -Wextra -Werror -Wpedantic -pedantic-errors -Wstrict-aliasing -Wno-variadic-macros")
# Relax slightly to temporarily allow unused parameters.
set(CMAKE_ERROR_WARNING_FLAGS "-std=c++20 -Wall -pthread -Wextra -Wpedantic -Wstrict-aliasing -Wno-variadic-macros")

# Actually set the g++ flags.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  ${CMAKE_ERROR_WARNING_FLAGS} ${CMAKE_CXX_FLAGS_RELEASE}")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS}  ${CMAKE_ERROR_WARNING_FLAGS} ${CMAKE_EXE_LINKER_FLAGS_RELEASE}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${CMAKE_ERROR_WARNING_FLAGS} ${CMAKE_EXE_LINKER_FLAGS_RELEASE}")

# Library and executable directories.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../..)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../lib)

set(RUNTIME_SOURCE_FILES
        kel_app/kel_exec_env.cpp
        kel_app/kel_exec_env.h
        kel_app/kel_exec_env_app.h
        kel_app/kel_logging.cpp
        kel_app/kel_logging.h
        kel_io/kel_line_record.h
        kel_io/kel_line_record.cpp
        kel_io/kel_bzip_workflow.cpp
        kel_io/kel_bzip_workflow.h
        kel_io/kel_bzip_workflow_verify.cpp
        kel_thread/kel_movefunction.h
        kel_thread/kel_queue_monitor.h
        kel_thread/kel_queue_mt_safe.h
        kel_thread/kel_queue_tidal.h
        kel_thread/kel_workflow_pipeline.h
        kel_thread/kel_workflow_threads.h
        kel_utility/kel_utility.cpp
        kel_utility/kel_utility.h
        )

add_library(kel_bgz STATIC ${RUNTIME_SOURCE_FILES})

add_executable(mt_bgz kel_bgz_main.cpp)

target_link_libraries (mt_bgz kel_bgz ${ZLIB_LIBRARIES})
