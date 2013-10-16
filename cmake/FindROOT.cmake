# AliRoot Build System Module to find and configure ROOT
#
# Author: Johny Jose m(johny.jose@cern.ch)
#         Port of previous Makefile build to cmake
#         Modified by Barthelemy von Haller


# TODO REVIEW THIS SCRIPT


cmake_minimum_required(VERSION 2.8.4 FATAL_ERROR)

find_program(ROOT_CONFIG NAMES root-config)
if(NOT ROOT_CONFIG)
  message(FATAL_ERROR "root-config is not installed on this system")
endif(NOT ROOT_CONFIG)


find_program(ROOTCINT NAMES rootcint)
if(NOT ROOTCINT)
  message(FATAL_ERROR "rootcint is not installed on this system")
endif(NOT ROOTCINT)

find_program(ROOTEXE NAMES root.exe)
if(NOT ROOTEXE)
  message(FATAL_ERROR "root.exe is not installed on this system")
endif(NOT ROOTEXE)

message(STATUS "Check for root-config: ${ROOT_CONFIG}")
message(STATUS "Check for rootcint: ${ROOTCINT}")
message(STATUS "Check for root.exe: ${ROOTEXE}")

execute_process(COMMAND ${ROOT_CONFIG} --incdir OUTPUT_VARIABLE ROOT_INCDIR )
execute_process(COMMAND ${ROOT_CONFIG} --libdir OUTPUT_VARIABLE ROOT_LIBDIR )
set(ROOTSYS $ENV{ROOTSYS} CACHE STRING "" FORCE)
if(NOT ROOTSYS)
  message(STATUS "Environment variable $ROOTSYS is not set !")
  string(REPLACE "rootcint" "" ROOTSYS ${ROOTCINT})
  string(REPLACE "bin" "" ROOTSYS ${ROOTSYS})
  message("Extracting ROOTSYS from ROOTCINT: ${ROOTSYS}")
else()
  get_filename_component(ROOTSYS "${ROOTSYS}" REALPATH)
endif(NOT ROOTSYS)

