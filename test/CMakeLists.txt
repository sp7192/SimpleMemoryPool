cmake_minimum_required(VERSION 3.14)

# Download and unpack googletest at configure time
configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
execute_process(COMMAND "${CMAKE_COMMAND}" -G "${CMAKE_GENERATOR}" .
    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/test/googletest-download"
)
execute_process(COMMAND "${CMAKE_COMMAND}" --build .
    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/test/googletest-download"
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(GTESTLIB "${PROJECT_SOURCE_DIR}/build/lib/Release/gtest.lib")

add_subdirectory("${CMAKE_BINARY_DIR}/test/googletest-src"
                 "${CMAKE_BINARY_DIR}/test/googletest-build"
)

include_directories("${gtest_SOURCE_DIR}/include"
                    "${gmock_SOURCE_DIR}/include"
					"../src"
)

enable_testing()

add_executable (SimpleMemoryPool 
				"main.cpp" 
				"../src/SimpleFixedMemoryPool.cpp"
				"../src/SimpleFixedMemoryPool.h"
				"TestSimpleFixedMemoryPool.h"
				"../src/MemoryBlock.h"
) 

target_link_libraries(
  SimpleMemoryPool
  ${GTESTLIB}
)
