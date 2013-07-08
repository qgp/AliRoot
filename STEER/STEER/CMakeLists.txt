cmake_minimum_required (VERSION 2.8)

# Module
set (MODULE STEER)
include_directories( ${ROOTSYS}/include ${CMAKE_SOURCE_DIR}/STEER/${MODULE})

# Additional include directories
include_directories( ${ROOTSYS}/include ${CMAKE_SOURCE_DIR}/STEER/STEERBase ${CMAKE_SOURCE_DIR}/STEER/CDB ${CMAKE_SOURCE_DIR}/STEER/ESD  ${CMAKE_SOURCE_DIR}/RAW/RAWDatabase ${CMAKE_SOURCE_DIR}/RAW/RAWDatarec ${CMAKE_SOURCE_DIR}/HLT/sim ${CMAKE_SOURCE_DIR}/ANALYSIS ${PROJECT_BINARY_DIR})

# Include directories for CLING
get_directory_property(incdirs INCLUDE_DIRECTORIES)
foreach( d ${incdirs})    
  set(include_dirs ${include_dirs} -I${d})
endforeach()

# Sources
set ( SRCS  
  AliRunLoader.cxx 
  AliLoader.cxx 
  AliDataLoader.cxx 
  AliObjectLoader.cxx 
  AliBaseLoader.cxx 
  AliTreeLoader.cxx 
  AliDetector.cxx 
  AliDigit.cxx 
  AliHit.cxx 
  AliRun.cxx 
  AliGenerator.cxx 
  AliVertexGenerator.cxx 
  AliLego.cxx	
  AliModule.cxx 
  AliDigitNew.cxx 
  AliGeometry.cxx 
  AliRecPoint.cxx 
  AliHitMap.cxx 
  AliLegoGenerator.cxx 
  AliLegoGeneratorXYZ.cxx 
  AliLegoGeneratorPhiZ.cxx 
  AliLegoGeneratorEta.cxx 
  AliLegoGeneratorEtaR.cxx 
  AliRndm.cxx 
  AliDebugVolume.cxx 
  AliConfig.cxx 
  AliDigitizationInput.cxx 
  AliDigitizer.cxx 
  AliStream.cxx 
  AliMergeCombi.cxx 
  AliGausCorr.cxx 
  AliVertexer.cxx 
  AliMC.cxx 
  AliSimulation.cxx 
  AliReconstruction.cxx 
  AliRecoInputHandler.cxx 
  AliVertexGenFile.cxx 
  AliReconstructor.cxx 
  AliGeomManager.cxx 
  AliAlignObj.cxx 
  AliAlignObjParams.cxx 
  AliAlignObjMatrix.cxx 
  AliMisAligner.cxx 
  AliTriggerDetector.cxx 
  AliCentralTrigger.cxx 
  AliRieman.cxx 
  AliTrackFitter.cxx 
  AliTrackFitterRieman.cxx 
  AliTrackFitterKalman.cxx 
  AliTrackFitterStraight.cxx 
  AliTrackResiduals.cxx 
  AliTrackResidualsChi2.cxx 
  AliTrackResidualsFast.cxx 
  AliTrackResidualsLinear.cxx 
  AliAlignmentTracks.cxx 
  AliCTPRawStream.cxx 
  AliSignalProcesor.cxx 
  AliTracker.cxx 
  AliCluster.cxx 
  AliCluster3D.cxx 
  AliTrackleter.cxx 
  AliHelix.cxx 
  AliV0.cxx 
  AliKink.cxx 
  AliSelectorRL.cxx 
  AliSurveyObj.cxx 
  AliSurveyPoint.cxx 
  AliSurveyToAlignObjs.cxx 
  AliFstream.cxx 
  AliCTPRawData.cxx 
  AliQADataMaker.cxx 
  AliGlobalQADataMaker.cxx 
  AliQADataMakerSim.cxx 
  AliQADataMakerRec.cxx 
  AliCorrQADataMakerRec.cxx 
  AliQAManager.cxx 
  AliQAChecker.cxx 
  AliCorrQAChecker.cxx 
  AliGlobalQAChecker.cxx 
  AliQACheckerBase.cxx 
  AliQAThresholds.cxx 
  AliMillepede.cxx 
  AliPlaneEff.cxx 
  AliTriggerRunScalers.cxx 
  AliGRPPreprocessor.cxx 
  AliGRPRecoParam.cxx 
  AliRelAlignerKalman.cxx 
  AliESDTagCreator.cxx 
  AliGRPObject.cxx 
  AliQAv1.cxx 
  AliRunInfo.cxx 
  AliEventInfo.cxx 
  AliRecoParam.cxx 
  AliDetectorRecoParam.cxx 
  AliMillePedeRecord.cxx 
  AliMillePede2.cxx 
  AliMatrixSq.cxx 
  AliVectorSparse.cxx 
  AliMatrixSparse.cxx 
  AliSymMatrix.cxx 
  AliSymBDMatrix.cxx 
  AliRectMatrix.cxx 
  AliMinResSolve.cxx 
  AliParamSolver.cxx 
  AliGRPManager.cxx 
  AliDCSArray.cxx 	 
  AliLHCReader.cxx 	 
  AliLHCDipValT.cxx 	 
  AliLHCData.cxx
  AliCTPTimeParams.cxx 
  AliCTPInputTimeParams.cxx 
  AliLHCClockPhase.cxx 
  AliLTUConfig.cxx
  AliTriggerUtils.cxx
  AliTransportMonitor.cxx
  AliParamList.cxx
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
