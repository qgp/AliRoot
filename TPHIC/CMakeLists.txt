cmake_minimum_required (VERSION 2.8)

# Module
set (MODULE TPHIC)
include_directories( ${ROOTSYS}/include ${CMAKE_SOURCE_DIR}/${MODULE})

# Additional include directories
include_directories( ${CMAKE_SOURCE_DIR}/STEER/STEERBase ${CMAKE_SOURCE_DIR}/STEER/STEER ${CMAKE_SOURCE_DIR}/EVGEN ${CMAKE_SOURCE_DIR}/PYTHIA6/AliPythia6 ${CMAKE_SOURCE_DIR}/LHAPDF)

# Include directories for CLING
get_directory_property(incdirs INCLUDE_DIRECTORIES)
foreach( d ${incdirs})    
  set(include_dirs ${include_dirs} -I${d})
endforeach()

# Sources
set ( SRCS 
  TPHICgen.cxx 
  AliGenTPHIC.cxx
  )

# Fortran sources
set ( FSRCS
  tphic17.f 
  )

# Headers from sources
string(REPLACE ".cxx" ".h" HDRS "${SRCS}")

# Make dictionary
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/G__${MODULE}.cxx ${CMAKE_CURRENT_BINARY_DIR}/G__${MODULE}.h
  COMMAND ${ROOTCINT} 
  ARGS -f ${CMAKE_CURRENT_BINARY_DIR}/G__${MODULE}.cxx -c -p 
  ${include_dirs}
  ${HDRS} ${MODULE}LinkDef.h
  DEPENDS ${HDRS} ${MODULE}LinkDef.h
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

# Add a library to the project using the specified source files
add_library(${MODULE} SHARED ${SRCS} G__${MODULE}.cxx ${FSRCS})

# Additional compilation flags
set_target_properties(${MODULE} PROPERTIES
  COMPILE_FLAGS "-O -g"
#  Fortran_FORMAT FREE
  )

# System dependent: Modify the way the library is build
if(${CMAKE_SYSTEM} MATCHES Darwin)
  set_target_properties(${MODULE} PROPERTIES
    LINK_FLAGS "-undefined dynamic_lookup"
    )
endif(${CMAKE_SYSTEM} MATCHES Darwin)

# Installation
install(TARGETS ${MODULE} 
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib)
install(FILES ${HDRS} DESTINATION include)
