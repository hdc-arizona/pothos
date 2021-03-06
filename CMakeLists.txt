cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

project(nanocube2
  VERSION 0.0.1
  DESCRIPTION "Pothos is a reimplementation of nanocubes"
  LANGUAGES CXX)

set (CMAKE_CXX_STANDARD 17)
find_package(Boost COMPONENTS thread chrono regex REQUIRED)

if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
endif()

# include(cmake/FindArrow.cmake) findarrow doesn't work, so we'll do it by hand
# let me say this will all my heart: fuck cmake

set (ARROW_BASE_PATH /Users/cscheid/repos/github/apache/arrow/cpp)
set (ARROW_INCLUDE_DIR ${ARROW_BASE_PATH}/src)
# set (ARROW_LIBRARY_DIR /Users/cscheid/repos/github/apache/arrow/cpp/release/release)
set (ARROW_LIBRARY_DIR ${ARROW_BASE_PATH}/release/release)

include_directories(AFTER $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> ${ARROW_INCLUDE_DIR})

################################################################################

set (POTHOS_BASE_FILES src/arrow_utils.cc src/debug.cc src/utils.cc)

add_library(pothos ${POTHOS_BASE_FILES})

################################################################################

add_executable(test_sparsesat
  src/test_sparsesat.cc src/sparsesat.cc src/debug.cc src/utils.cc)

add_executable(test_garbagecube
  src/garbagecube.cc src/test_garbagecube.cc src/naivecube.cc src/debug.cc src/utils.cc)

add_executable(test_nanocube
  src/nanocube.cc src/test_nanocube.cc src/naivecube.cc src/debug.cc src/utils.cc)

add_executable(test_sparsesatcube
  src/garbagecube.cc src/sparsesatcube.cc src/test_sparsesatcube.cc
  src/naivecube.cc src/nanocube.cc src/debug.cc src/utils.cc)

################################################################################

add_executable(test_nyc_pickup
  src/garbagecube.cc src/nanocube.cc
  src/sparsesat.cc
  src/test_nyc_pickup.cc src/debug.cc src/utils.cc)

target_link_libraries(test_nyc_pickup ${Boost_LIBRARIES})

################################################################################

add_executable(test_arrow
  src/test_arrow.cc
  src/arrow_utils.cc
  )

target_link_directories(test_arrow PUBLIC ${ARROW_LIBRARY_DIR})

target_link_libraries(test_arrow PUBLIC arrow)

################################################################################

add_executable(test_htm
  src/test_htm.cc
  )

################################################################################

add_executable(test_ztf_arrow
  src/test_ztf_arrow.cc
  src/arrow_utils.cc
  )

target_link_directories(test_ztf_arrow PUBLIC ${ARROW_LIBRARY_DIR})

target_link_libraries(test_ztf_arrow PUBLIC arrow)
