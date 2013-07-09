cmake_minimum_required (VERSION 2.8)

# Module
set (MODULE ANALYSISalice)
include_directories( ${ROOTSYS}/include ${CMAKE_SOURCE_DIR}/ANALYSIS/${MODULE})

# Additional include directories
include_directories( ${ROOTSYS}/include ${CMAKE_SOURCE_DIR}/STEER/STEERBase ${CMAKE_SOURCE_DIR}/ANALYSIS/ANALYSIS ${CMAKE_SOURCE_DIR}/STEER/ESD ${CMAKE_SOURCE_DIR}/STEER/AOD ${CMAKE_SOURCE_DIR}/OADB)

# Include directories for CLING
get_directory_property(incdirs INCLUDE_DIRECTORIES)
foreach( d ${incdirs})    
  set(include_dirs ${include_dirs} -I${d})
endforeach()

# Sources
set ( SRCS  	
  AliAnalysisTaskSE.cxx 
  AliAnalysisTaskME.cxx 
  AliAnalysisTaskESDfilter.cxx 
  AliAnalysisTaskMCParticleFilter.cxx 
  AliKineTrackCuts.cxx 
  AliESDtrackCuts.cxx 
  AliESDpidCuts.cxx 
  AliESDv0Cuts.cxx 
  AliEventPoolOTF.cxx 
  AliEventPoolLoop.cxx 
  AliEventPoolSparse.cxx 
  AliAnalysisTaskTagCreator.cxx 
  AliMultiEventInputHandler.cxx 
  AliTriggerAnalysis.cxx 
  AliTRDTriggerAnalysis.cxx 
  AliPhysicsSelection.cxx 
  AliBackgroundSelection.cxx 
  AliPhysicsSelectionTask.cxx 
  AliAnalysisFilter.cxx 
  AliAnalysisCuts.cxx 
  AliCollisionNormalization.cxx 
  AliCollisionNormalizationTask.cxx 
  AliCentralitySelectionTask.cxx 
  AliEPSelectionTask.cxx 
  AliVZEROEPSelectionTask.cxx 
  AliAnalysisTaskStat.cxx 
  AliMultiInputEventHandler.cxx 
  AliESDv0KineCuts.cxx 
  AliAnalysisTaskPIDResponse.cxx 
  AliPIDResponseInputHandler.cxx 
  AliHEPDataParser.cxx 
  AliAnalysisTaskPIDqa.cxx 
  AliAnalysisTaskBaseLine.cxx
  AliEventPoolManager.cxx
  AliAnalysisTaskPIDCombined.cxx
  AliUnfolding.cxx
  AliAnalysisUtils.cxx
  AliAnalysisTaskBadChunkID.cxx
  AliTagAnalysis.cxx 
  AliXMLCollection.cxx 
  AliAnalysisAlien.cxx
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
install(TARGETS ${MODULE} LIBRARY DESTINATION lib)
install(FILES ${HDRS} DESTINATION include)
