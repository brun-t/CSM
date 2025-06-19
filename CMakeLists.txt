cmake_minimum_required(VERSION 3.14)
project(CSM LANGUAGES C)

add_library(CSM INTERFACE)

target_include_directories(CSM INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:include>
)

install(TARGETS CSM EXPORT CSMTargets)

install(FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/CSM.h
  DESTINATION include
)

install(EXPORT CSMTargets
  FILE CSMTargets.cmake
  NAMESPACE CSM::
  DESTINATION lib/cmake/CSM
)
