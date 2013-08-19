cmake_minimum_required (VERSION 2.8)

# Module
set (MODULE MUONrec)
include_directories( ${ROOTSYS}/include ${CMAKE_SOURCE_DIR}/MUON/${MODULE})

# Additional include directories
include_directories( ${ROOTSYS}/include ${CMAKE_SOURCE_DIR}/STEER/STEERBase ${CMAKE_SOURCE_DIR}/STEER/STEER ${CMAKE_SOURCE_DIR}/STEER/ESD ${CMAKE_SOURCE_DIR}/STEER/CDB ${CMAKE_SOURCE_DIR}/MUON/MUONbase ${CMAKE_SOURCE_DIR}/MUON/MUONcore ${CMAKE_SOURCE_DIR}/MUON/MUONcalib ${CMAKE_SOURCE_DIR}/MUON/MUONgeometry ${CMAKE_SOURCE_DIR}/MUON/MUONmapping ${CMAKE_SOURCE_DIR}/MUON/MUONtrigger ${CMAKE_SOURCE_DIR}/MUON/MUONraw ${CMAKE_SOURCE_DIR}/RAW/RAWDatarec ${CMAKE_SOURCE_DIR}/RAW/RAWDatabase)

# Include directories for CLING
get_directory_property(incdirs INCLUDE_DIRECTORIES)
foreach( d ${incdirs})    
  set(include_dirs ${include_dirs} -I${d})
endforeach()

# Sources
set ( SRCS  
    AliMUONReconstructor.cxx 
    AliMUONVTrackReconstructor.cxx 
    AliMUONTrackReconstructor.cxx 
    AliMUONTrackReconstructorK.cxx 
    AliMUONTracker.cxx 
    AliMUONTrack.cxx 
    AliMUONTrackParam.cxx 
    AliMUONTrackExtrap.cxx 
    AliMUONTriggerTrack.cxx 
    AliMUONVTriggerTrackStore.cxx 
    AliMUONTriggerTrackStoreV1.cxx 
    AliMUONVClusterFinder.cxx 
    AliMUONPreClusterFinder.cxx 
    AliMUONPreClusterFinderV2.cxx 
    AliMUONPreClusterFinderV3.cxx 
    AliMUONClusterFinderMLEM.cxx 
    AliMUONClusterSplitterMLEM.cxx 
    AliMUONClusterFinderSimpleFit.cxx 
    AliMUONClusterFinderCOG.cxx 
    AliMUONPad.cxx 
    AliMUONCluster.cxx 
    AliMUONTrackHitPattern.cxx 
    AliMUONVClusterStore.cxx 
    AliMUONClusterStoreV1.cxx 
    AliMUONClusterStoreV2.cxx 
    AliMUONClusterStoreV2Iterator.cxx 
    AliMUONVTrackStore.cxx 
    AliMUONTrackStoreV1.cxx 
    AliMUONVClusterServer.cxx 
    AliMUONSimpleClusterServer.cxx 
    AliMUONVQADataMakerRec.cxx 
    AliMUONQADataMakerRec.cxx 
    AliMUONTrackerQADataMakerRec.cxx 
    AliMUONTriggerQADataMakerRec.cxx 
    AliMUONClusterFinderPeakCOG.cxx 
    AliMUONClusterFinderPeakFit.cxx 
    AliMUONRefitter.cxx 
    AliMUONESDInterface.cxx 
    AliMUONLegacyClusterServer.cxx 
    AliMUONTriggerTrackToTrackerClusters.cxx 
    AliMUONVTrackerDataMaker.cxx 
    AliMUONTrackerDataMaker.cxx 
    AliMUONQAMappingCheck.cxx 
    AliMUONCDB.cxx 
    AliMUONTrackerConditionDataMaker.cxx 
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
