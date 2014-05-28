cmake_minimum_required (VERSION 2.8.8 FATAL_ERROR)
project (AliRoot CXX C)

# If no Fortran, i.e on Windows
include(CheckLanguage)
check_language(Fortran)
if(CMAKE_Fortran_COMPILER)
  enable_language(Fortran OPTIONAL)
else()
  message(STATUS "No Fortran support")
endif()
# Specify the languages of the project
# Sets the following variables:
#   CMAKE_CXX_COMPILER
#   AliRoot_BINARY_DIR
#   AliRoot_SOURCE_DIR

message("CMAKE_CXX_COMPILER: ${CMAKE_CXX_COMPILER}")
message("AliRoot_BINARY_DIR: ${AliRoot_BINARY_DIR}")
message("AliRoot_SOURCE_DIR: ${AliRoot_SOURCE_DIR}")
message("CMAKE_SYSTEM: ${CMAKE_SYSTEM}")
# Set the default install dir to the source directory
IF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    SET(CMAKE_INSTALL_PREFIX ${AliRoot_SOURCE_DIR} CACHE PATH "Install in the
    source directory if CMAKE_INSTALL_PREFIX is not specified" FORCE)
ENDIF()
message("CMAKE_INSTALL_PREFIX: ${CMAKE_INSTALL_PREFIX}")

# Additional modules
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# Search for Root
find_package(ROOT REQUIRED)

# Git
find_package(Git)
if(GIT_FOUND)
    if(EXISTS ${PROJECT_SOURCE_DIR}/.git)
      execute_process(COMMAND "${GIT_EXECUTABLE}" describe --all
	WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
	OUTPUT_VARIABLE ALIROOTURL
	OUTPUT_STRIP_TRAILING_WHITESPACE)
	execute_process(COMMAND "${GIT_EXECUTABLE}" describe --always
	WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
	OUTPUT_VARIABLE   ALIROOTREV
	OUTPUT_STRIP_TRAILING_WHITESPACE)
    else()
      set(ALIROOTURL "Undefined")
      set(ALIROOTREV "Undefined")
    endif()
endif()

#Create ARVersion.h
configure_file (
    "${PROJECT_SOURCE_DIR}/ARVersion.h.in"
    "${PROJECT_BINARY_DIR}/ARVersion.h"
    )

# Root include directories
include_directories( ${ROOT_INCDIR} ${CMAKE_SOURCE_DIR})

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
add_subdirectory (ZDC)
add_subdirectory (STRUCT)
add_subdirectory (HMPID)
add_subdirectory (PMD)
add_subdirectory (MUON)
add_subdirectory (FMD)
add_subdirectory (ACORDE)
add_subdirectory (EMCAL)
add_subdirectory (PHOS)
add_subdirectory (TRIGGER)
add_subdirectory (BCM)
add_subdirectory (CORRFW)
add_subdirectory (FASTSIM)
if(CMAKE_Fortran_COMPILER_WORKS)
  add_subdirectory (LHAPDF)
  add_subdirectory (PYTHIA6)
  add_subdirectory (MICROCERN)
endif(CMAKE_Fortran_COMPILER_WORKS)
add_subdirectory (EVGEN)
add_subdirectory (TUHKMgen)
add_subdirectory (TTherminator)
add_subdirectory (TPHIC)
add_subdirectory (THydjet)
add_subdirectory (PWG)
add_subdirectory (Vc)
add_subdirectory (HLT)

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
