cmake_minimum_required (VERSION 2.8)

# Module
set (MODULE FMDsim)
include_directories( ${ROOTSYS}/include ${CMAKE_SOURCE_DIR}/FMD/${MODULE})

# Additional include directories
include_directories( ${ROOTSYS}/include ${CMAKE_SOURCE_DIR}/STEER/STEERBase ${CMAKE_SOURCE_DIR}/STEER/STEER ${CMAKE_SOURCE_DIR}/STEER/ESD ${CMAKE_SOURCE_DIR}/FMD/FMDbase ${CMAKE_SOURCE_DIR}/FMD/FMDrec ${CMAKE_SOURCE_DIR}/RAW/RAWDatasim ${CMAKE_SOURCE_DIR}/RAW/RAWDatarec ${CMAKE_SOURCE_DIR}/RAW/RAWDatabase)

# Include directories for CLING
get_directory_property(incdirs INCLUDE_DIRECTORIES)
foreach( d ${incdirs})    
  set(include_dirs ${include_dirs} -I${d})
endforeach()

# Sources
set ( SRCS   
  AliFMD.cxx 
  AliFMDv0.cxx 
  AliFMDv1.cxx AliFMDHit.cxx
  AliFMDBaseDigitizer.cxx
  AliFMDDigitizer.cxx
  AliFMDHitDigitizer.cxx
  AliFMDEdepMap.cxx
  AliFMDRawWriter.cxx
  AliFMDQADataMakerSim.cxx)

# Headers from sources
string(REPLACE ".cxx" ".h" HDRS "${SRCS}")

# Additional header files with no implementation
set(HDRS ${HDRS} AliFMDEdepHitPair.h)

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
add_library(${MODULE} SHARED ${SRCS} G__${MODULE}.cxx)

# Additional compilation flags
set_target_properties(${MODULE} PROPERTIES
  COMPILE_FLAGS "-O -g"
  )

# System dependent: Modify the way the library is build
if(${CMAKE_SYSTEM} MATCHES Darwin)
  set_target_properties(${MODULE} PROPERTIES
    LINK_FLAGS "-undefined dynamic_lookup"
    )
endif(${CMAKE_SYSTEM} MATCHES Darwin)

# Installation
install(TARGETS ${MODULE} LIBRARY DESTINATION lib)
install(FILES ${HDRS} DESTINATION include)
