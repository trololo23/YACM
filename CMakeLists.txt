cmake_minimum_required(VERSION 3.10)

project(YACM)

# find ncurses library
find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

# add executable
add_executable(myproject main.c src/windows.c src/files.c src/system.c src/errhandler.c)

# link against ncurses library
target_link_libraries(myproject ${CURSES_LIBRARIES})
