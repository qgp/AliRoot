cmake_minimum_required (VERSION 2.8)

include ("${CMAKE_SOURCE_DIR}/HLT/CMakehlt.conf")

# Module
set (MODULE AliHLTMUON)
include_directories( ${ROOTSYS}/include ${CMAKE_SOURCE_DIR}/HLT/MUON ${CMAKE_SOURCE_DIR}/HLT/MUON/OfflineInterface ${CMAKE_SOURCE_DIR}/HLT/MUON/OnlineAnalysis ${CMAKE_SOURCE_DIR}/HLT/MUON/utils)

# Additional include directories
include_directories( ${CMAKE_SOURCE_DIR}/STEER/STEERBase ${CMAKE_SOURCE_DIR}/STEER/ESD ${CMAKE_SOURCE_DIR}/STEER/CDB ${CMAKE_SOURCE_DIR}/STEER/STEER ${CMAKE_SOURCE_DIR}/HLT/BASE ${CMAKE_SOURCE_DIR}/MUON/MUONraw ${CMAKE_SOURCE_DIR}/MUON/MUONsim ${CMAKE_SOURCE_DIR}/MUON/MUONevaluation ${CMAKE_SOURCE_DIR}/MUON/MUONbase ${CMAKE_SOURCE_DIR}/MUON/MUONrec ${CMAKE_SOURCE_DIR}/MUON/MUONcore ${CMAKE_SOURCE_DIR}/MUON/MUONmapping ${CMAKE_SOURCE_DIR}/MUON/MUONtrigger ${CMAKE_SOURCE_DIR}/MUON/MUONgeometry ${CMAKE_SOURCE_DIR}/MUON/MUONcalib ${CMAKE_SOURCE_DIR}/RAW/RAWDatarec ${CMAKE_SOURCE_DIR}/RAW/RAWDatabase ${CMAKE_SOURCE_DIR}/RAW/RAWDatasim)# ${CMAKE_SOURCE_DIR}/HLT/BASE/HOMER)

# Include directories for CLING
get_directory_property(incdirs INCLUDE_DIRECTORIES)
foreach( d ${incdirs})    
  set(include_dirs ${include_dirs} -I${d})
endforeach()

# Sources
set ( SRCS 
  AliHLTMUONUtils.cxx
  OfflineInterface/AliHLTMUONAgent.cxx
  OfflineInterface/AliHLTMUONTriggerRecordsSource.cxx
  OfflineInterface/AliHLTMUONRecHitsSource.cxx
  OfflineInterface/AliHLTMUONDigitPublisherComponent.cxx
  OfflineInterface/AliHLTMUONRootifierComponent.cxx
  OfflineInterface/AliHLTMUONESDMaker.cxx
  OnlineAnalysis/AliHLTMUONTriggerReconstructorComponent.cxx
  OnlineAnalysis/AliHLTMUONHitReconstructorComponent.cxx
  OnlineAnalysis/AliHLTMUONMansoTrackerFSMComponent.cxx
  OnlineAnalysis/AliHLTMUONFullTrackerComponent.cxx
  OnlineAnalysis/AliHLTMUONDecisionComponent.cxx
  OnlineAnalysis/AliHLTMUONClusterFinderComponent.cxx
  utils/AliHLTMUONEmptyEventFilterComponent.cxx
  utils/AliHLTMUONDataCheckerComponent.cxx
  utils/AliHLTMUONRawDataHistoComponent.cxx
  utils/AliHLTMUONClusterHistoComponent.cxx
  AliHLTMUONConstants.cxx
  AliHLTMUONProcessor.cxx
  AliHLTMUONRecHit.cxx
  AliHLTMUONTriggerRecord.cxx
  AliHLTMUONTrack.cxx
  AliHLTMUONMansoTrack.cxx
  AliHLTMUONDecision.cxx
  AliHLTMUONEvent.cxx
  )

# Headers from sources
string(REPLACE ".cxx" ".h" HDRS "${SRCS}")

# Additional header files with no implementation
set(HDRS ${HDRS}
  AliHLTMUONDataBlockReader.h
  AliHLTMUONDataBlockWriter.h
  AliHLTMUONDataTypes.h
  AliHLTMUONList.h
  AliHLTMUONCountedList.h
  OnlineAnalysis/AliHLTMUONMansoTrackerFSMCallback.h
  ) 
# Additional implementation files with no header
set(SRCS ${SRCS} 
  AliHLTMUONTriggerRecordsBlockStruct.cxx
  AliHLTMUONTrigRecsDebugBlockStruct.cxx
  AliHLTMUONRecHitsBlockStruct.cxx
  AliHLTMUONClustersBlockStruct.cxx
  AliHLTMUONChannelsBlockStruct.cxx
  AliHLTMUONMansoTracksBlockStruct.cxx
  AliHLTMUONMansoCandidatesBlockStruct.cxx
  AliHLTMUONTracksBlockStruct.cxx
  AliHLTMUONSinglesDecisionBlockStruct.cxx
  AliHLTMUONPairsDecisionBlockStruct.cxx
  OnlineAnalysis/AliHLTMUONCalculations.cxx
  OnlineAnalysis/AliHLTMUONTriggerReconstructor.cxx
  OnlineAnalysis/AliHLTMUONHitReconstructor.cxx
  OnlineAnalysis/AliHLTMUONMansoTrackerFSM.cxx
  OnlineAnalysis/AliHLTMUONFullTracker.cxx
  )

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
