cmake_minimum_required (VERSION 2.8)

include ("${CMAKE_SOURCE_DIR}/HLT/CMakehlt.conf")

# Module
set (MODULE AliHLTCalo)
include_directories( ${ROOTSYS}/include ${CMAKE_SOURCE_DIR}/HLT/CALO ${CMAKE_SOURCE_DIR}/HLT/CALO/offline)

# Additional include directories
include_directories( ${CMAKE_SOURCE_DIR}/STEER/STEERBase ${CMAKE_SOURCE_DIR}/STEER/ESD ${CMAKE_SOURCE_DIR}/STEER/CDB ${CMAKE_SOURCE_DIR}/STEER/STEER ${CMAKE_SOURCE_DIR}/EMCAL/EMCALUtils ${CMAKE_SOURCE_DIR}/EMCAL/EMCALraw ${CMAKE_SOURCE_DIR}/HLT/BASE ${CMAKE_SOURCE_DIR}/HLT/BASE/util ${CMAKE_SOURCE_DIR}/RAW/RAWDatarec ${CMAKE_SOURCE_DIR}/RAW/RAWDatabase ${CMAKE_SOURCE_DIR}/HLT/TPCLib ${CMAKE_SOURCE_DIR}/HLT/EMCAL/offline ${CMAKE_SOURCE_DIR}/HLT/EMCAL ${CMAKE_SOURCE_DIR}/HLT/PHOS/offline ${CMAKE_SOURCE_DIR}/HLT/PHOS)

# Include directories for CLING
get_directory_property(incdirs INCLUDE_DIRECTORIES)
foreach( d ${incdirs})    
  set(include_dirs ${include_dirs} -I${d})
endforeach()

# Sources
set ( SRCS 
  AliHLTCaloRawAnalyzerComponentv3.cxx
  AliHLTCaloUtilities.cxx
  AliHLTCaloMapper.cxx
  AliHLTCaloDefinitions.cxx
  AliHLTCaloConstants.cxx
  AliHLTCaloSanityInspector.cxx
  AliHLTCaloSharedMemoryInterfacev2.cxx
  AliHLTCaloFourier.cxx
  AliHLTCaloConstantsHandler.cxx
  AliHLTCaloClusterizer.cxx
  AliHLTCaloClusterizerNbyN.cxx
  AliHLTCaloClusterizerComponent.cxx
  AliHLTCaloDigitMaker.cxx
  AliHLTCaloClusterAnalyser.cxx
  AliHLTCaloProcessor.cxx
  AliHLTCaloGeometry.cxx
  AliHLTCaloRecoParamHandler.cxx
  offline/AliHLTCaloDigitPublisherComponent.cxx
  offline/AliHLTCaloDigitHandler.cxx
  )

# Headers from sources
string(REPLACE ".cxx" ".h" HDRS "${SRCS}")

# Additional header files with no implementation
set(HDRS ${HDRS} AliHLTCaloDigitContainerDataStruct.h AliHLTCaloRecPointDataStruct.h AliHLTCaloRecPointContainerStruct.h AliHLTCaloRecPointHeaderStruct.h)

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
