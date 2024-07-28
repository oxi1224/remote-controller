cmake_minimum_required(VERSION 3.24)

project(remote-controller-client)

option(TEST_BUILD "Enable testing build" OFF)
option(DEV_BUILD "Enable developer build" ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB SRC "src/*.cpp")
file(GLOB INCLUDE_SRC "include/*.hpp")

if(TEST_BUILD)
  list(REMOVE_ITEM SRC "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")
endif()

if(NOT DEV_BUILD)
  message(STATUS "Configuring for production build")
  set(CMAKE_BUILD_TYPE Release)
endif()

add_executable(${CMAKE_PROJECT_NAME} ${SRC} ${INCLUDE_SRC})
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE "include")

if (WIN32) 
  target_link_libraries(${CMAKE_PROJECT_NAME} ws2_32)
endif()

if (TEST_BUILD)
  include(FetchContent)
  FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG       "v1.15.0" 
  )

  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
  FetchContent_MakeAvailable(googletest)

  enable_testing()

  file(GLOB TEST_SRC "test/*_test.cpp")

  target_link_libraries(
    ${CMAKE_PROJECT_NAME}
    GTest::gtest_main
  )

  include(GoogleTest)

  foreach(TEST_FILE ${TEST_SRC})
    get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)
    target_sources(${CMAKE_PROJECT_NAME} PRIVATE ${TEST_FILE})
    add_test(${TEST_NAME} ${CMAKE_PROJECT_NAME})
  endforeach()
endif()
