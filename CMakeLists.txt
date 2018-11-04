cmake_minimum_required(VERSION 3.11)
project(QR VERSION 0.1 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(EXECUTABLE_OUTPUT_PATH .)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY lib)

file(GLOB header include/*.h)
file(GLOB source src/*.c)

add_library(qr ${header} ${source})
target_include_directories(qr
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/include
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/src
        )

add_executable(QR main.c)
target_link_libraries(QR qr)
