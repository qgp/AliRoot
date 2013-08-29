cmake_minimum_required (VERSION 2.8)

include ("${CMAKE_SOURCE_DIR}/HLT/CMakehlt.conf")

# Module
set (MODULE AliHLTUtil)
include_directories( ${ROOTSYS}/include ${CMAKE_SOURCE_DIR}/HLT/BASE/util)

# Additional include directories
include_directories( ${CMAKE_SOURCE_DIR}/STEER/STEERBase ${CMAKE_SOURCE_DIR}/STEER/AOD ${CMAKE_SOURCE_DIR}/STEER/ESD ${CMAKE_SOURCE_DIR}/STEER/CDB ${CMAKE_SOURCE_DIR}/STEER/STEER ${CMAKE_SOURCE_DIR}/RAW/RAWDatarec ${CMAKE_SOURCE_DIR}/ANALYSIS/ANALYSISalice)

# Include directories for CLING
get_directory_property(incdirs INCLUDE_DIRECTORIES)
foreach( d ${incdirs})    
  set(include_dirs ${include_dirs} -I${d})
endforeach()

# Sources
set ( SRCS 
  AliHLTFilePublisher.cxx
  AliHLTFileWriter.cxx
  AliHLTMCEvent.cxx
  AliHLTJets.cxx
  AliHLTRootFilePublisherComponent.cxx
  AliHLTESDMCEventPublisherComponent.cxx
  AliHLTRootFileWriterComponent.cxx
  AliHLTRootFileStreamerComponent.cxx
  AliHLTRootSchemaEvolutionComponent.cxx
  AliHLTRecoParamComponent.cxx
  AliHLTLoaderPublisherComponent.cxx
  AliHLTRawReaderPublisherComponent.cxx
  AliHLTOUTPublisherComponent.cxx
  AliHLTEsdCollectorComponent.cxx
  AliHLTReadoutListDumpComponent.cxx
  AliHLTDataGenerator.cxx
  AliHLTBlockFilterComponent.cxx
  AliHLTMonitoringRelay.cxx
  AliHLTObjectCompressionComponent.cxx
  AliHLTCompStatCollector.cxx
  AliHLTGlobalBarrelTrack.cxx
  AliHLTAgentUtil.cxx
  AliHLTESDCaloClusterMaker.cxx
  AliHLTCaloClusterReader.cxx
  AliHLTCorruptorComponent.cxx
  AliHLTESDTrackCuts.cxx
  )

# Headers from sources
string(REPLACE ".cxx" ".h" HDRS "${SRCS}")

# Additional header files with no implementation
# set(HDRS ${HDRS} AliHLTHOMERData.h)

# Additional definitions
# add_definitions("-DUSE_ROOT")

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
