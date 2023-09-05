cmake_minimum_required(VERSION 3.16)
set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_COMPILER "gcc")
set(CMAKE_C_STANDARD 11)

add_compile_options(
    -O1
)

project(brainfreak)

set(APP_SOURCE_DIR ${PROJECT_SOURCE_DIR}/src)
set(files
    ${APP_SOURCE_DIR}/main.c
    ${APP_SOURCE_DIR}/bf.c
    ${APP_SOURCE_DIR}/opt.c
)

add_executable(${PROJECT_NAME} ${files})

target_include_directories(${PROJECT_NAME}
    PRIVATE ${APP_SOURCE_DIR}/include
)