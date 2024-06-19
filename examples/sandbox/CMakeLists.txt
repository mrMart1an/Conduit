cmake_minimum_required(VERSION 3.2)

project(
    "Sandbox"
    VERSION 1.0.0
    LANGUAGES CXX
)

add_executable(${PROJECT_NAME} "src/sandbox.cpp")
target_link_libraries(${PROJECT_NAME} LINK_PUBLIC ${ENGINE_NAME})
