cmake_minimum_required (VERSION 2.8)
project (AliRoot CXX)
# Specify the languages of the project
# Sets the following variables:
#   CMAKE_CXX_COMPILER
#   AliRoot_BINARY_DIR
#   AliRoot_SOURCE_DIR

message("CMAKE_CXX_COMPILER: ${CMAKE_CXX_COMPILER}")
message("AliRoot_BINARY_DIR: ${AliRoot_BINARY_DIR}")
message("AliRoot_SOURCE_DIR: ${AliRoot_SOURCE_DIR}")
message("CMAKE_SYSTEM: ${CMAKE_SYSTEM}")
# Temporary installation in the source tree 
set(CMAKE_INSTALL_PREFIX ${AliRoot_SOURCE_DIR})
message("CMAKE_INSTALL_PREFIX: ${CMAKE_INSTALL_PREFIX}")

# Additional modules
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# Search for Root
find_package(ROOT REQUIRED)

# Subversion
find_package(Subversion)
if(SUBVERSION_FOUND)
  if(EXISTS ${PROJECT_SOURCE_DIR}/.svn)
    Subversion_WC_INFO(${PROJECT_SOURCE_DIR} Project)
    set(ALIROOTURL ${Project_WC_URL})
    set(ALIROOTREV ${Project_WC_REVISION})
  endif(EXISTS ${PROJECT_SOURCE_DIR}/.svn)
endif(SUBVERSION_FOUND)

#Create ARVersion.h
configure_file (
  "${PROJECT_SOURCE_DIR}/ARVersion.h.in"
  "${PROJECT_BINARY_DIR}/ARVersion.h"
  )

# Root include directories
include_directories( ${ROOTSYS}/include ${CMAKE_SOURCE_DIR})

# include the libraries
add_subdirectory (STEER)
add_subdirectory (STAT)
add_subdirectory (OADB)
add_subdirectory (ANALYSIS)
add_subdirectory (RAW)
add_subdirectory (ITS)
add_subdirectory (TPC)
add_subdirectory (VZERO)
add_subdirectory (T0)
add_subdirectory (TOF)
add_subdirectory (TRD)

# Installation
install(FILES ${PROJECT_BINARY_DIR}/ARVersion.h DESTINATION include)

# build a CPack driven installer package
include (InstallRequiredSystemLibraries)
set (CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/License.txt")
set (CPACK_PACKAGE_VERSION_MAJOR "5.04")
set (CPACK_PACKAGE_VERSION_MINOR "01")
set (CPACK_PACKAGE_CONTACT       "Peter.Hristov@cern.ch")
set (CPACK_PACKAGING_INSTALL_PREFIX "/opt/aliroot")
include (CPack)
