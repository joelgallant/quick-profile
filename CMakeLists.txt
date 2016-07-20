cmake_minimum_required(VERSION 2.8)
project(QuickProfile)
get_directory_property(HAS_PARENT PARENT_DIRECTORY)

set(PROJECT_SOURCE_DIR "${PROJECT_SOURCE_DIR}/src")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(QUICK_PROFILE_INCLUDE "${PROJECT_SOURCE_DIR}")
if (HAS_PARENT)
    set(QUICK_PROFILE_INCLUDE "${QUICK_PROFILE_INCLUDE}" PARENT_SCOPE)
endif()

add_library(profile src/profile)
