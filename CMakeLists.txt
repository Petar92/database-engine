cmake_minimum_required(VERSION 3.21)
project(database-engine)

enable_language(CXX)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set up the vcpkg toolchain file
set(CMAKE_TOOLCHAIN_FILE "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/vcpkg/scripts/buildsystems/vcpkg.cmake" 
    CACHE STRING "Vcpkg toolchain file")

# Find Boost
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
find_package(Boost REQUIRED COMPONENTS system)

# Debugging output
message(STATUS "Boost_FOUND: ${Boost_FOUND}")
message(STATUS "Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
message(STATUS "Boost_LIBRARIES: ${Boost_LIBRARIES}")

find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

# Handle executable name differences across platforms
if (WIN32)
    find_program(BISON_EXECUTABLE win_bison REQUIRED)
    find_program(FLEX_EXECUTABLE win_flex REQUIRED)
else()
    find_program(BISON_EXECUTABLE bison REQUIRED)
    find_program(FLEX_EXECUTABLE flex REQUIRED)
endif()

BISON_TARGET(QueryParser
    ${CMAKE_SOURCE_DIR}/src/QueryParser/query.y
    ${CMAKE_BINARY_DIR}/query.tab.cpp
    DEFINES_FILE ${CMAKE_BINARY_DIR}/query.hpp
)

if (WIN32)
  set(FLEX_FLAGS "--wincompat")
else()
  set(FLEX_FLAGS "")
endif()

FLEX_TARGET(QueryParser
  ${CMAKE_SOURCE_DIR}/src/QueryParser/query.l
  ${CMAKE_BINARY_DIR}/query.yy.cpp
  COMPILE_FLAGS ${FLEX_FLAGS}
)


ADD_FLEX_BISON_DEPENDENCY(QueryParser QueryParser)

set_source_files_properties(${BISON_QueryParser_OUTPUTS} PROPERTIES LANGUAGE CXX)
set_source_files_properties(${FLEX_QueryParser_OUTPUTS} PROPERTIES LANGUAGE CXX)

add_executable(database-engine
    "src/Main.cpp"
    ${BISON_QueryParser_OUTPUTS}
    ${FLEX_QueryParser_OUTPUTS}
    "src/ProcessPool/Worker.h" 
    "src/ProcessPool/Worker.cpp"
    "src/ProcessPool/ProcessPool.h" 
    "src/ProcessPool/ProcessPool.cpp" 
    "src/Handlers/CommandHandler.cpp" 
    "src/Handlers/CommandHandler.h"
    "src/FileManager/FileManager.cpp" 
    "src/FileManager/FileManager.h"
 "src/TCPServer/Server.h" "src/TCPServer/Server.cpp")

target_include_directories(database-engine PRIVATE
  ${CMAKE_SOURCE_DIR}/src/Handlers
  ${CMAKE_SOURCE_DIR}/src/FileManager
  ${CMAKE_BINARY_DIR}
  ${Boost_INCLUDE_DIRS}
)
target_link_libraries(database-engine PRIVATE ${Boost_LIBRARIES})