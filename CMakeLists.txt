cmake_minimum_required(VERSION 3.22.1)
project(Rofi-Themer)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_STANDARD_REQUIRED true)
# SET(GCC_COVERAGE_COMPILE_FLAGS "-O2 -Wall -Wextra -pedantic -std=c++17")


set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g3")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -Wall -Wextra -pedantic -std=c++17")


# SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}") # ???????
#SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS}")

if(DEFINED SOCK_PATH)
	add_definitions(-DSOCK_PATH="${SOCK_PATH}")
else()
    message(STATUS "MY_DEFINE_VALUE is not set. You can set it using: cmake -DMY_DEFINE_VALUE=<value> <path_to_source>")
endif()


add_executable(Rofi-Themer-daemon
        src/daemon/main.cpp
		src/daemon/data.cpp
		src/daemon/entry.cpp
		src/daemon/table.cpp
		src/daemon/list.cpp
		src/daemon/common.cpp
)

target_include_directories(Rofi-Themer-daemon
		PUBLIC include/common        
		PUBLIC include/daemon
)

add_executable(Rofi-Themer-menu
		src/menu/main.cpp
)

target_include_directories(Rofi-Themer-menu
		PUBLIC include/common        
		PUBLIC include/menu
)

add_executable(Rofi-Themer-read
        src/read/main.cpp
)

target_include_directories(Rofi-Themer-read
		PUBLIC include/common        
		PUBLIC include/read
)
