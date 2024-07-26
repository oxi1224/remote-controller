cmake_minimum_required(VERSION 3.24)

project(remote-controller-client)

option(DEV_BUILD "Enable development build" ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB SRC "src/*.cpp")
file(GLOB INCLUDE_SRC "include/*.hpp")

add_executable(${CMAKE_PROJECT_NAME} ${SRC} ${INCLUDE_SRC})

if (WIN32) 
  target_link_libraries(${CMAKE_PROJECT_NAME} ws2_32)
endif ()

if(DEV_BUILD)
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

  target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE "include")
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
else()
  message(STATUS "Configuring for production build")
  set(CMAKE_BUILD_TYPE Release)
endif()

