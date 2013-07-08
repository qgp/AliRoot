cmake_minimum_required (VERSION 2.8)

# Module
set (MODULE STEERBase)
include_directories(${CMAKE_SOURCE_DIR}/STEER/${MODULE})

# Include directories for CLING
get_directory_property(incdirs INCLUDE_DIRECTORIES)
foreach( d ${incdirs})    
  set(include_dirs ${include_dirs} -I${d})
endforeach()

# Sources
set ( SRCS  
  AliVParticle.cxx 
  AliVTrack.cxx 
  AliEventplane.cxx 
  AliVCluster.cxx 
  AliVCaloCells.cxx 
  AliVVertex.cxx 
  AliVEvent.cxx 
  AliMixedEvent.cxx 
  AliVHeader.cxx 
  AliVEventHandler.cxx 
  AliVEventPool.cxx 
  AliVCuts.cxx 
  AliVVZERO.cxx 
  AliVZDC.cxx 
  AliCentrality.cxx 
  AliPID.cxx 
  AliLog.cxx 
  AliRunTag.cxx 
  AliLHCTag.cxx 
  AliDetectorTag.cxx 
  AliEventTag.cxx 
  AliFileTag.cxx 
  AliEventTagCuts.cxx 
  AliRunTagCuts.cxx 
  AliLHCTagCuts.cxx 
  AliDetectorTagCuts.cxx 
  AliTagCreator.cxx 
  AliHeader.cxx 
  AliGenEventHeader.cxx 
  AliDetectorEventHeader.cxx 
  AliGenPythiaEventHeader.cxx 
  AliGenCocktailEventHeader.cxx 
  AliGenGeVSimEventHeader.cxx 
  AliGenHijingEventHeader.cxx 
  AliCollisionGeometry.cxx 
  AliGenDPMjetEventHeader.cxx 
  AliGenHerwigEventHeader.cxx 
  AliGenEposEventHeader.cxx 
  AliStack.cxx 
  AliMCEventHandler.cxx 
  AliInputEventHandler.cxx 
  AliTrackReference.cxx 
  AliSysInfo.cxx 
  AliMCEvent.cxx 
  AliMCParticle.cxx 
  AliMCVertex.cxx 
  AliMagF.cxx 
  AliMagWrapCheb.cxx 
  AliCheb3D.cxx 
  AliCheb3DCalc.cxx 
  AliNeutralTrackParam.cxx 
  AliCodeTimer.cxx 
  AliPDG.cxx 
  AliTimeStamp.cxx 
  AliTriggerScalers.cxx 
  AliTriggerScalersRecord.cxx 
  AliExternalTrackParam.cxx 
  AliQA.cxx 
  AliITSPidParams.cxx 
  AliTRDPIDReference.cxx 
  AliTRDPIDParams.cxx 
  AliTRDPIDResponseObject.cxx
  AliTRDTKDInterpolator.cxx
  AliPIDResponse.cxx 
  AliITSPIDResponse.cxx 
  AliTPCPIDResponse.cxx 
  AliTOFPIDResponse.cxx 
  AliTOFPIDParams.cxx 
  AliTRDPIDResponse.cxx 
  AliEMCALPIDResponse.cxx 
  AliHMPIDPIDResponse.cxx
  AliHMPIDPIDParams.cxx
  AliPIDCombined.cxx
  AliPIDValues.cxx
  AliDetectorPID.cxx
  AliDAQ.cxx 
  AliRefArray.cxx
  AliOADBContainer.cxx	
  AliMathBase.cxx 
  TTreeStream.cxx 
  AliVMFT.cxx
  AliTPCdEdxInfo.cxx
  AliCounterCollection.cxx
  AliTOFHeader.cxx
  AliTOFTriggerMask.cxx
  AliVCaloTrigger.cxx
  AliProdInfo.cxx
  AliVTrdTrack.cxx
  AliVTrdTracklet.cxx
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
