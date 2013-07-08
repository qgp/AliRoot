cmake_minimum_required (VERSION 2.8)

# Module
set (MODULE ITSbase)
include_directories( ${ROOTSYS}/include ${CMAKE_SOURCE_DIR}/ITS/${MODULE})

# Additional include directories
include_directories( ${ROOTSYS}/include ${CMAKE_SOURCE_DIR}/STEER/STEERBase ${CMAKE_SOURCE_DIR}/STEER/STEER ${CMAKE_SOURCE_DIR}/STEER/ESD ${CMAKE_SOURCE_DIR}/STEER/CDB ${CMAKE_SOURCE_DIR}/RAW/RAWDatarec ${CMAKE_SOURCE_DIR}/RAW/RAWDatabase ${CMAKE_SOURCE_DIR}/RAW ${CMAKE_SOURCE_DIR}/RAW/RAWDatasim ${CMAKE_SOURCE_DIR}/ITS/ITSrec ${CMAKE_SOURCE_DIR}/ITS/ITSsim)

# Include directories for CLING
get_directory_property(incdirs INCLUDE_DIRECTORIES)
foreach( d ${incdirs})    
  set(include_dirs ${include_dirs} -I${d})
endforeach()

# Sources
set ( SRCS 
  AliITSgeom.cxx 
  AliITSgeomMatrix.cxx 
  AliITSInitGeometry.cxx 
  AliITSLoader.cxx 
  AliITSRecPoint.cxx 
  AliITSclusterV2.cxx 
  AliITSdigit.cxx 
  AliITSdigitSPD.cxx 
  AliITSdigitSDD.cxx 
  AliITSTransientDigit.cxx 
  AliITSdigitSSD.cxx 
  AliITSMap.cxx 
  AliITSMapA1.cxx 
  AliITSMapA2.cxx 
  AliITSMisAligner.cxx 
  AliITSpList.cxx 
  AliITSpListItem.cxx 
  AliITSsegmentation.cxx 
  AliITSsegmentationSPD.cxx 
  AliITSsegmentationSDD.cxx 
  AliITSsegmentationSSD.cxx 
  AliITSCalibration.cxx 
  AliITSresponse.cxx 
  AliITSresponseSPD.cxx 
  AliITSresponseSDD.cxx 
  AliITSCalibrationSPD.cxx 
  AliITSCalibrationSDD.cxx 
  AliITSCalibrationSSD.cxx 
  AliITSChannelStatus.cxx 
  AliITSMapSDD.cxx 
  AliITSCorrMapSDD.cxx 
  AliITSCorrMap1DSDD.cxx 
  AliITSCorrMap2DSDD.cxx 
  AliITSHLTforSDD.cxx 
  AliITSDriftSpeedSDD.cxx 
  AliITSDDLModuleMapSDD.cxx 
  AliITSDriftSpeedArraySDD.cxx 
  AliITSBadChannelsSSD.cxx 
  AliITSBadChannelsSSDv2.cxx 
  AliITSGainSSD.cxx 
  AliITSGainSSDv2.cxx 
  AliITSNoiseSSD.cxx 
  AliITSNoiseSSDv2.cxx 
  AliITSChannelDaSSD.cxx 
  AliITSModuleDaSSD.cxx 
  AliITSHandleDaSSD.cxx 
  AliITSDDLRawData.cxx 
  AliITSRawStream.cxx 
  AliITSRawStreamSDDCompressed.cxx 
  AliITSRawStreamSSD.cxx 
  AliITSRawStreamSDD.cxx 
  AliITSCompressRawDataSDD.cxx 
  AliITSRawStreamSPD.cxx 
  AliITSEventHeader.cxx 
  AliITSRawStreamSSDv1.cxx 
  AliITSgeomTGeo.cxx 
  AliITSPlaneEff.cxx 
  AliITSPlaneEffSPD.cxx 
  AliITSPlaneEffSDD.cxx 
  AliITSPlaneEffSSD.cxx 
  AliITSRawStreamSPDErrorLog.cxx 
  AliITSDCSAnalyzerSDD.cxx 
  AliITSDCSDataSDD.cxx 
  AliITSIntMap.cxx 
  AliITSIntMapNode.cxx 
  AliITSPedestalSSD.cxx 
  AliITSPedestalSSDv2.cxx 
  AliITSSurveyToAlign.cxx 
  AliITSTriggerConditions.cxx 
  AliITSTriggerAlgorithmConditions.cxx 
  AliITSdEdxSamples.cxx 
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
  COMPILE_FLAGS "-O -g -Dlong32='int' -Dlong64='long long' -DdatePointer='long'"
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
