cmake_minimum_required(VERSION 2.8.7)
project(ctmpl)

include_directories(${PROJECT_SOURCE_DIR})

set(CMAKE_CXX_COMPILER clang++)

set(APP_SRC
  main.cpp
  )

add_executable(ctmpl ${APP_SRC})
