cmake_minimum_required (VERSION 2.8)

include ("${CMAKE_SOURCE_DIR}/HLT/CMakehlt.conf")

# Module
set (MODULE HLTbase)
include_directories( ${ROOTSYS}/include ${CMAKE_SOURCE_DIR}/HLT/BASE)

# Additional include directories
include_directories( ${CMAKE_SOURCE_DIR}/HLT/BASE/HOMER ${CMAKE_SOURCE_DIR}/RAW/RAWDatabase)

# Include directories for CLING
get_directory_property(incdirs INCLUDE_DIRECTORIES)
foreach( d ${incdirs})    
  set(include_dirs ${include_dirs} -I${d})
endforeach()

# Sources
set ( SRCS 
    AliHLTComponent.cxx
    AliHLTComponentHandler.cxx
    AliHLTMisc.cxx
    AliHLTSystem.cxx
    AliHLTReconstructorBase.cxx
    AliHLTPluginBase.cxx
    AliHLTProcessor.cxx
    AliHLTCalibrationProcessor.cxx
    AliHLTConfiguration.cxx
    AliHLTComponentConfiguration.cxx
    AliHLTConfigurationHandler.cxx
    AliHLTOnlineConfiguration.cxx
    AliHLTTTreeProcessor.cxx
    AliHLTTask.cxx
    AliHLTDumpTask.cxx
    AliHLTControlTask.cxx
    AliHLTLogging.cxx
    AliHLTErrorGuard.cxx
    AliHLTDataBuffer.cxx
    AliHLTConsumerDescriptor.cxx
    AliHLTDataSource.cxx
    AliHLTDataSink.cxx
    AliHLTOfflineInterface.cxx
    AliHLTOfflineDataSource.cxx
    AliHLTModuleAgent.cxx
    AliHLTModulePreprocessor.cxx
    AliHLTShuttleInterface.cxx
    AliHLTDimServer.cxx
    AliHLTHOMERLibManager.cxx
    AliHLTHOMERManager.cxx
    AliHLTHOMERProxyHandler.cxx
    AliHLTHOMERBlockDesc.cxx
    AliHLTHOMERSourceDesc.cxx
    AliHLTEsdManager.cxx
    AliHLTDAQ.cxx
    AliHLTOUT.cxx
    AliHLTOUTHomerBuffer.cxx
    AliHLTOUTTask.cxx
    AliHLTOUTHandler.cxx
    AliHLTOUTHandlerIgnore.cxx
    AliHLTOUTHandlerEquId.cxx
    AliHLTOUTHandlerDetectorDDL.cxx
    AliHLTOUTHandlerChain.cxx
    AliHLTOUTHandlerEsdBranch.cxx
    AliHLTMemoryFile.cxx
    AliHLTMessage.cxx
    AliHLTEventStatistics.cxx
    AliHLTBlockDataCollection.cxx
    AliHLTTriggerDecision.cxx
    AliHLTGlobalTriggerDecision.cxx
    AliHLTReadoutList.cxx
    AliHLTTriggerDomain.cxx
    AliHLTDomainEntry.cxx
    AliHLTTriggerMenu.cxx
    AliHLTTriggerMenuItem.cxx
    AliHLTTriggerMenuSymbol.cxx
    AliHLTCTPData.cxx
    AliHLTScalars.cxx
    AliHLTRunStatistics.cxx
    AliHLTSpacePointContainer.cxx
    AliHLTTrackGeometry.cxx
    AliHLTComponentBenchmark.cxx
    AliHLTDataDeflater.cxx
    AliHLTDataDeflaterSimple.cxx
    AliHLTDataDeflaterHuffman.cxx
    AliHLTHuffman.cxx
    AliHLTDataInflater.cxx
    AliHLTDataInflaterSimple.cxx
    AliHLTDataInflaterHuffman.cxx
    AliHLT_C_Component_WrapperInterface.cxx 
    AliHLTDataTypes.cxx
    )

# Headers from sources
string(REPLACE ".cxx" ".h" HDRS "${SRCS}")

# Additional header files with no implementation
set(HDRS ${HDRS} 
  AliHLTLoggingVariadicFree.h 
  AliHLTExternalTrackParam.h 
  AliHLTCommonCDBEntries.h 
  AliHLTDefinitions.h 
  AliHLTStdIncludes.h
  )

# Definitions
add_definitions(${HLTDEFS})

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

add_subdirectory(HOMER)
add_subdirectory(interface)
add_subdirectory(util)
