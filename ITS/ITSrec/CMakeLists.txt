cmake_minimum_required (VERSION 2.8)

# Module
set (MODULE ITSrec)
include_directories( ${ROOTSYS}/include ${CMAKE_SOURCE_DIR}/ITS/${MODULE})

# Additional include directories
include_directories( ${ROOTSYS}/include ${CMAKE_SOURCE_DIR}/STEER/STEERBase ${CMAKE_SOURCE_DIR}/STEER/ESD ${CMAKE_SOURCE_DIR}/STEER/STEER  ${CMAKE_SOURCE_DIR}/ITS/ITSbase  ${CMAKE_SOURCE_DIR}/STEER/CDB  ${CMAKE_SOURCE_DIR}/RAW/RAWDatarec  ${CMAKE_SOURCE_DIR}/RAW/RAWDatabase  ${CMAKE_SOURCE_DIR}/ITS/ITSsim)

# Include directories for CLING
get_directory_property(incdirs INCLUDE_DIRECTORIES)
foreach( d ${incdirs})    
  set(include_dirs ${include_dirs} -I${d})
endforeach()

# Sources
set ( SRCS 
  AliITSDetTypeRec.cxx 
  AliITSClusterFinder.cxx 
  AliITSClusterFinderV2SDD.cxx 
  AliITSClusterFinderV2SPD.cxx 
  AliITSClusterFinderV2SSD.cxx 
  AliITSClusterParam.cxx 
  AliITSCorrectSDDPoints.cxx 
  AliITSclustererV2.cxx 
  AliITSIOTrack.cxx 
  AliITSRad.cxx 
  AliITStrackV2.cxx 
  AliITStrackerV2.cxx 
  AliITStrackMI.cxx 
  AliITStrackerMI.cxx 
  AliITSV0Finder.cxx 
  AliITSVertexer.cxx 
  AliITSVertexerCosmics.cxx 
  AliITSVertexer3D.cxx 
  AliITSTracklPairs.cxx 
  AliITSSortTrkl.cxx 
  AliITSVertexerZ.cxx 
  AliITSVertexerZD.cxx
  AliITSVertexerFast.cxx 
  AliITSVertexerFixed.cxx 
  AliITSVertexer3DTapan.cxx 
  AliITSMeanVertexer.cxx 
  AliITSZPoint.cxx 
  AliITSreconstruction.cxx 
  AliITSclusterTable.cxx 
  AliITStrackSA.cxx 
  AliITStrackerSA.cxx 
  AliITSdEdxAnalyzer.cxx 
  AliITSReconstructor.cxx 
  AliITSRecoParam.cxx 
  AliITSMultReconstructor.cxx 
  AliITSPreprocessorSPD.cxx 
  AliITSOnlineCalibrationSPD.cxx 
  AliITSOnlineCalibrationSPDhandler.cxx 
  AliITSOnlineSPDHitArray.cxx 
  AliITSOnlineSPDHitEvent.cxx 
  AliITSOnlineSPDscanAnalyzer.cxx 
  AliITSOnlineSPDscan.cxx 
  AliITSOnlineSPDscanInfo.cxx 
  AliITSOnlineSPDscanInfoMeanTh.cxx 
  AliITSOnlineSPDscanInfoMultiple.cxx 
  AliITSOnlineSPDscanMeanTh.cxx 
  AliITSOnlineSPDscanMultiple.cxx 
  AliITSOnlineSPDscanSingle.cxx 
  AliITSOnlineSPDphys.cxx 
  AliITSOnlineSPDphysAnalyzer.cxx 
  AliITSOnlineSPDphysInfo.cxx 
  AliITSOnlineSPDfoChipConfig.cxx 
  AliITSOnlineSPDfoChip.cxx 
  AliITSOnlineSPDfo.cxx 
  AliITSOnlineSPDfoInfo.cxx 
  AliITSOnlineSPDfoAnalyzer.cxx 
  AliITSBadChannelsAuxSPD.cxx 
  AliITSBadChannelsSPD.cxx 
  AliITSChannelSPD.cxx 
  AliITSPreprocessorSDD.cxx 
  AliITSOnlineSDD.cxx 
  AliITSOnlineSDDBase.cxx 
  AliITSOnlineSDDTP.cxx 
  AliITSOnlineSDDInjectors.cxx 
  AliITSOnlineSDDCMN.cxx 
  AliITSPreprocessorSSD.cxx 
  AliITSAlignMille.cxx 
  AliITSAlignMille2.cxx 
  AliITSAlignMilleModule.cxx 
  AliITSAlignMilleData.cxx 
  AliITSAlignMille2Module.cxx 
  AliITSAlignMille2Constraint.cxx 
  AliITSAlignMille2ConstrArray.cxx 
  AliITSTPArrayFit.cxx 
  AliITSRealignTracks.cxx 
  AliITSQAChecker.cxx 
  AliITSQADataMakerRec.cxx 
  AliITSQASPDDataMakerRec.cxx 
  AliITSQASDDDataMakerRec.cxx 
  AliITSQASSDDataMakerRec.cxx 
  AliITSQASPDChecker.cxx 
  AliITSQASDDChecker.cxx 
  AliITSQASSDChecker.cxx 
  AliITSQASSDRefData.cxx 
  AliITSTrackleterSPDEff.cxx 
  AliITSRecPointContainer.cxx
  AliITSSumTP.cxx 
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
