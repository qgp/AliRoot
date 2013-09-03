cmake_minimum_required (VERSION 2.8)

include ("${CMAKE_SOURCE_DIR}/HLT/CMakehlt.conf")
include ("${CMAKE_SOURCE_DIR}/Vc/Vc.cmake")
ALICE_UseVc()

# Module
set (MODULE AliHLTTPC)
include_directories( ${ROOTSYS}/include ${CMAKE_SOURCE_DIR}/HLT/TPCLib ${CMAKE_SOURCE_DIR}/HLT/TPCLib/transform ${CMAKE_SOURCE_DIR}/HLT/TPCLib/offline ${CMAKE_SOURCE_DIR}/HLT/TPCLib/tracking-ca ${CMAKE_SOURCE_DIR}/HLT/TPCLib/merger-ca ${CMAKE_SOURCE_DIR}/HLT/TPCLib/comp ${CMAKE_SOURCE_DIR}/HLT/TPCLib/HWCFemulator)

# Additional include directories
include_directories( ${CMAKE_SOURCE_DIR}/HLT/BASE ${CMAKE_SOURCE_DIR}/HLT/BASE/util ${CMAKE_SOURCE_DIR}/HLT/RCU ${CMAKE_SOURCE_DIR}/STEER/STEERBase ${CMAKE_SOURCE_DIR}/STEER/ESD ${CMAKE_SOURCE_DIR}/STEER/CDB ${CMAKE_SOURCE_DIR}/STEER/STEER ${CMAKE_SOURCE_DIR}/TPC/TPCbase ${CMAKE_SOURCE_DIR}/TPC/TPCrec ${CMAKE_SOURCE_DIR}/TPC/TPCsim ${CMAKE_SOURCE_DIR}/Vc/include ${CMAKE_SOURCE_DIR}/RAW/RAWDatabase ${CMAKE_SOURCE_DIR}/RAW/RAWDatarec ${CMAKE_SOURCE_DIR}/ANALYSIS/ANALYSISalice)

# Include directories for CLING
get_directory_property(incdirs INCLUDE_DIRECTORIES)
foreach( d ${incdirs})    
  set(include_dirs ${include_dirs} -I${d})
endforeach()

# Sources
set ( SRCS 
  AliHLTTPCTransform.cxx
  transform/AliHLTTPCSpline2D3D.cxx
  transform/AliHLTTPCFastTransform.cxx
  AliHLTTPCClusterTransformation.cxx
  AliHLTTPCClusterTransformationComponent.cxx
  AliHLTTPCAgent.cxx
  AliHLTTPCMemHandler.cxx
  AliHLTTPCFileHandler.cxx
  AliHLTTPCClusterFinder.cxx
  AliHLTTPCMapping.cxx
  AliHLTTPCDigitReader.cxx
  AliHLTTPCDigitReaderUnpacked.cxx
  AliHLTTPCDigitReader32Bit.cxx
  AliHLTTPCDigitDumpComponent.cxx
  AliHLTTPCDataCheckerComponent.cxx
  AliHLTTPCSpacePointContainer.cxx
  AliHLTTPCRawSpacePointContainer.cxx
  AliHLTTPCHWCFSpacePointContainer.cxx
  AliHLTTPCTrackGeometry.cxx
  AliHLTTPCHWCFData.cxx
  offline/AliHLTTPCDigitPublisherComponent.cxx
  offline/AliHLTTPCOfflineCluster.cxx
  AliHLTTPCClusters.cxx
  AliHLTTPCPad.cxx
  AliHLTTPCDefinitions.cxx
  AliHLTTPCClusterFinderComponent.cxx
  AliHLTTPCHWClusterTransformComponent.cxx
  AliHLTTPCCFComparisonComponent.cxx
  AliHLTTPCHistogramHandlerComponent.cxx
  AliHLTTPCClusterConverterComponent.cxx
  AliHLTTPCClusterAccessHLTOUT.cxx
  tracking-ca/AliHLTTPCCATrack.cxx
  tracking-ca/AliHLTTPCCATracklet.cxx
  tracking-ca/AliHLTTPCCATracker.cxx
  tracking-ca/AliHLTTPCCABaseTrackParam.cxx
  tracking-ca/AliHLTTPCCATrackParam.cxx
  tracking-ca/AliHLTTPCCATrackConvertor.cxx
  tracking-ca/AliHLTTPCCAParam.cxx
  tracking-ca/AliHLTTPCCARow.cxx
  tracking-ca/AliHLTTPCCAGrid.cxx
  tracking-ca/AliHLTTPCCAHitArea.cxx
  tracking-ca/AliHLTTPCCAMCPoint.cxx
  tracking-ca/AliHLTTPCCATrackerComponent.cxx
  tracking-ca/AliHLTTPCCANeighboursFinder.cxx
  tracking-ca/AliHLTTPCCANeighboursCleaner.cxx
  tracking-ca/AliHLTTPCCAStartHitsFinder.cxx
  tracking-ca/AliHLTTPCCATrackletConstructor.cxx
  tracking-ca/AliHLTTPCCATrackletSelector.cxx
  tracking-ca/AliHLTTPCCADisplay.cxx
  tracking-ca/AliHLTTPCCAMCTrack.cxx
  tracking-ca/AliHLTTPCCAPerformance.cxx
  tracking-ca/AliHLTTPCCAMerger.cxx
  tracking-ca/AliHLTTPCCAGlobalMergerComponent.cxx
  tracking-ca/AliHLTTPCCAClusterData.cxx
  tracking-ca/AliHLTTPCCASliceData.cxx
  tracking-ca/AliHLTTPCCASliceOutput.cxx
  tracking-ca/AliHLTTPCCAStandaloneFramework.cxx
  tracking-ca/AliTPCtrackerCA.cxx
  tracking-ca/AliHLTTPCCATrackerFramework.cxx
  tracking-ca/AliHLTTPCCAInputDataCompressorComponent.cxx
  tracking-ca/AliHLTTPCCAGPUTracker.cxx
  tracking-ca/AliHLTTPCCATrackerOutputConverter.cxx
  merger-ca/AliHLTTPCGMTrackParam.cxx
  merger-ca/AliHLTTPCGMSliceTrack.cxx
  merger-ca/AliHLTTPCGMMerger.cxx
  comp/AliHLTTPCDataCompressionComponent.cxx
  comp/AliHLTTPCDataCompressionMonitorComponent.cxx
  comp/AliHLTTPCDataCompressionDecoder.cxx
  comp/AliHLTTPCDataCompressionFilterComponent.cxx
  AliHLTTPCDataPublisherComponent.cxx
  AliHLTTPCClusterHistoComponent.cxx
  AliHLTTPCTrackHistoComponent.cxx
  AliHLTTPCTrackDumpComponent.cxx
  AliHLTTPCClusterDumpComponent.cxx
  AliHLTTPCHWCFDataReverterComponent.cxx
  AliHLTTPCTrackMCMarkerComponent.cxx
  AliHLTTPCdEdxComponent.cxx
  AliHLTTPCdEdxMonitoringComponent.cxx
  HWCFemulator/AliHLTTPCHWCFExtractorUnit.cxx
  HWCFemulator/AliHLTTPCHWCFPeakFinderUnit.cxx
  HWCFemulator/AliHLTTPCHWCFProcessorUnit.cxx
  HWCFemulator/AliHLTTPCHWCFMergerUnit.cxx
  HWCFemulator/AliHLTTPCHWCFDivisionUnit.cxx
  HWCFemulator/AliHLTTPCHWCFEmulator.cxx
  HWCFemulator/AliHLTTPCHWCFSupport.cxx
  HWCFemulator/AliHLTTPCHWCFEmulatorComponent.cxx
  HWCFemulator/AliHLTTPCHWCFConsistencyControlComponent.cxx
  HWCFemulator/AliHLTTPCHWClusterMerger.cxx
  HWCFemulator/AliHLTTPCHWClusterMergerV1.cxx
  AliHLTTPCHWClusterDecoderComponent.cxx
  )

# Headers from sources
string(REPLACE ".cxx" ".h" HDRS "${SRCS}")

# Additional implementation files with no header
set(SRCS ${SRCS}  AliHLTTPCLog.cxx)

# Additional header files with no implementation
set(HDRS ${HDRS}  
  AliHLTTPCLog.h
  AliHLTTPCLogging.h
  AliHLTTPCSpacePointData.h
  AliHLTTPCDigitData.h
  AliHLTTPCTrackSegmentData.h
  AliHLTTPCTrackletDataFormat.h
  AliHLTTPCClusterDataFormat.h
  AliHLTTPCClusterMCData.h
  HWCFemulator/AliHLTTPCHWCFDataTypes.h
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

# Link with libVc.a
target_link_libraries(${MODULE} Vc)

# Additional compilation flags
set_target_properties(${MODULE} PROPERTIES
  COMPILE_FLAGS "-Wshadow -Wall -O3 -g"
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
