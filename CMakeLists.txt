cmake_minimum_required(VERSION 3.14)
project(sqlite_crossplatform)

enable_language(C CXX)

find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

# Handle executable name differences across platforms
if (WIN32)
    # Use win_bison.exe / win_flex.exe 
    find_program(BISON_EXECUTABLE win_bison REQUIRED)
    find_program(FLEX_EXECUTABLE win_flex REQUIRED)
else()
    # Use regular bison/flex on Linux/WSL/macOS
    find_program(BISON_EXECUTABLE bison REQUIRED)
    find_program(FLEX_EXECUTABLE flex REQUIRED)
endif()

BISON_TARGET(QueryParser
    ${CMAKE_SOURCE_DIR}/src/QueryParser/query.y
    ${CMAKE_BINARY_DIR}/query.tab.c
)

FLEX_TARGET(QueryParser
    ${CMAKE_SOURCE_DIR}/src/QueryParser/query.l
    ${CMAKE_BINARY_DIR}/query.yy.c
)

ADD_FLEX_BISON_DEPENDENCY(QueryParser QueryParser)

add_executable(sqlite
    src/Server.cpp
    ${BISON_QueryParser_OUTPUTS}
    ${FLEX_QueryParser_OUTPUTS}
 "src/ExecutionContext/ExecutionContext.h" "src/ProcessPool/Worker.h" "src/ProcessPool/ProcessPool.h" "src/ProcessPool/ProcessPool.cpp" "src/ProcessPool/Command.h" "src/ProcessPool/Worker.cpp")

target_include_directories(sqlite PRIVATE ${CMAKE_BINARY_DIR})
