cmake_minimum_required(VERSION 3.14)

project(remote-controller-client)

option(DEV_BUILD "Enable development build" ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB SRC "src/*.cpp")
file(GLOB INCLUDE_SRC "include/*.hpp")

add_executable(${CMAKE_PROJECT_NAME} "main.cpp" ${SRC} ${INCLUDE_SRC})

if(DEV_BUILD)
  include(FetchContent)
  FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
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

