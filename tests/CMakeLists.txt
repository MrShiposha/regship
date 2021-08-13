cmake_minimum_required(VERSION 3.19)
project(test-reg-ship)

file(GLOB SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/*.cpp
)

list(REMOVE_ITEM SRC "${CMAKE_CURRENT_SOURCE_DIR}/../src/main.cpp")

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../deps/catch2/src
)

add_executable(${PROJECT_NAME} ${SRC})
target_link_libraries(${PROJECT_NAME} PRIVATE Catch2::Catch2WithMain)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)
