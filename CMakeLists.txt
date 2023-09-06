cmake_minimum_required(VERSION 3.22)
set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_COMPILER "gcc")
set(CMAKE_C_STANDARD 11)

# set( CMAKE_VERBOSE_MAKEFILE on )

project(brainfreak)

# flags processing
if(${CMAKE_BUILD_DEBUG} MATCHES "true")
    add_compile_options(-O0 -ggdb)
else()
    add_compile_options(-O1)
endif()


set(APP_SOURCE_DIR ${PROJECT_SOURCE_DIR}/src)
set(files
    ${APP_SOURCE_DIR}/bf.c
    ${APP_SOURCE_DIR}/opt.c
)

if(${CMAKE_BUILD_TYPE} MATCHES "executable")
    add_executable( ${PROJECT_NAME} 
        ${files} ${APP_SOURCE_DIR}/main.c
    )

elseif(${CMAKE_BUILD_TYPE} MATCHES "shared")
    add_library( ${PROJECT_NAME} SHARED
        ${files}
    )
elseif(${CMAKE_BUILD_TYPE} MATCHES "static")
    add_library( ${PROJECT_NAME} STATIC
        ${files}
    )
else()
    message(FATAL_ERROR "invalid build type '" ${CMAKE_BUILD_TYPE} "'. exiting.")
endif()

target_include_directories(${PROJECT_NAME}
    PRIVATE ${APP_SOURCE_DIR}/include
)

unset(CMAKE_BUILD_DEBUG CACHE)
unset(CMAKE_BUILD_TYPE CACHE)