cmake_minimum_required(VERSION 3.14)

# Download and unpack googletest at configure time
configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
execute_process(COMMAND "${CMAKE_COMMAND}" -G "${CMAKE_GENERATOR}" .
    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/src/googletest-download"
)
execute_process(COMMAND "${CMAKE_COMMAND}" --build .
    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/src/googletest-download"
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(GMOCKLIB "${PROJECT_SOURCE_DIR}/build/lib/Release/gmock.lib")
set(GTESTLIB "${PROJECT_SOURCE_DIR}/build/lib/Release/gtest.lib")

add_subdirectory("${CMAKE_BINARY_DIR}/src/googletest-src"
                 "${CMAKE_BINARY_DIR}/src/googletest-build"
)

include_directories("${gtest_SOURCE_DIR}/include"
                    "${gmock_SOURCE_DIR}/include"
)

enable_testing()

add_executable (SimpleMemoryPool 
				"main.cpp" 
				"SimpleMemoryPool.cpp"
				"SimpleMemoryPool.h"
				"MemoryBlock.h"
) 

target_link_libraries(
  SimpleMemoryPool
  ${GMOCKLIB}
  ${GTESTLIB}
)
