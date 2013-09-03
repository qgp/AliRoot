cmake_minimum_required (VERSION 2.8)

include ("${CMAKE_SOURCE_DIR}/HLT/CMakehlt.conf")

# Module
set (MODULE AliHLTPHOS)
include_directories( ${ROOTSYS}/include ${CMAKE_SOURCE_DIR}/HLT/PHOS ${CMAKE_SOURCE_DIR}/HLT/PHOS/offline)

# Additional include directories
include_directories( ${CMAKE_SOURCE_DIR}/STEER/STEERBase ${CMAKE_SOURCE_DIR}/STEER/CDB ${CMAKE_SOURCE_DIR}/STEER/STEER ${CMAKE_SOURCE_DIR}/HLT/CALO ${CMAKE_SOURCE_DIR}/EMCAL/EMCALUtils ${CMAKE_SOURCE_DIR}/EMCAL/EMCALraw ${CMAKE_SOURCE_DIR}/PHOS/PHOSUtils ${CMAKE_SOURCE_DIR}/PHOS/PHOSrec ${CMAKE_SOURCE_DIR}/PHOS/PHOSbase ${CMAKE_SOURCE_DIR}/HLT/BASE ${CMAKE_SOURCE_DIR}/HLT/BASE/util ${CMAKE_SOURCE_DIR}/RAW/RAWDatabase)

# Include directories for CLING
get_directory_property(incdirs INCLUDE_DIRECTORIES)
foreach( d ${incdirs})    
  set(include_dirs ${include_dirs} -I${d})
endforeach()

# Sources
set ( SRCS 
  AliHLTPHOSClusterizerComponent.cxx
  AliHLTPHOSClusterizerComponentNbyN.cxx
  AliHLTPHOSDefinitions.cxx
  AliHLTPHOSRawAnalyzer.cxx
  AliHLTPHOSRawAnalyzerComponentv3.cxx
  AliHLTPHOSRawAnalyzerCrude.cxx
  AliHLTPHOSRawAnalyzerCrudeComponent.cxx
  AliHLTPHOSDigitMakerComponent.cxx
  AliHLTPHOSMapper.cxx
  AliHLTPHOSRawAnalyzerPeakFinder.cxx
  AliHLTPHOSSanityInspector.cxx
  AliHLTPHOSMonitorTriggerComponent.cxx
  AliHLTPHOSConstants.cxx
  AliHLTPHOSGeometry.cxx
  AliHLTPHOSRecoParamHandler.cxx
  offline/AliHLTPHOSDigitHandler.cxx
  )

# Headers from sources
string(REPLACE ".cxx" ".h" HDRS "${SRCS}")

# Additional header files with no implementation
set(HDRS ${HDRS} AliHLTPHOSCommonDefs.h AliHLTPHOSCoordinate.h)

# Definitions
add_definitions("${HLTDEFS}")

# Definitions for CLING
get_directory_property(defs DEFINITIONS)
separate_arguments(defs)
foreach( dd ${defs})    
  set(CLINGDEFS ${CLINGDEFS} ${dd})
endforeach()

# Make dictionary
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/G__${MODULE}.cxx ${CMAKE_CURRENT_BINARY_DIR}/G__${MODULE}.h
  COMMAND ${ROOTCINT} 
  ARGS -f ${CMAKE_CURRENT_BINARY_DIR}/G__${MODULE}.cxx -c -p 
  ${include_dirs}
  ${CLINGDEFS}
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
install(TARGETS ${MODULE}
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib)
install(FILES ${HDRS} DESTINATION include)
