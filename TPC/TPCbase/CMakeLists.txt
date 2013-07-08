cmake_minimum_required (VERSION 2.8)

# Module
set (MODULE TPCbase)
include_directories( ${ROOTSYS}/include ${CMAKE_SOURCE_DIR}/TPC/${MODULE})

# Additional include directories
include_directories( ${ROOTSYS}/include ${CMAKE_SOURCE_DIR}/STEER/STEERBase ${CMAKE_SOURCE_DIR}/STEER/STEER ${CMAKE_SOURCE_DIR}/STEER/CDB ${CMAKE_SOURCE_DIR}/RAW/RAWDatabase ${CMAKE_SOURCE_DIR}/RAW/RAWDatarec ${CMAKE_SOURCE_DIR}/STEER/ESD ${CMAKE_SOURCE_DIR}/RAW ${PROJECT_BINARY_DIR} ${CMAKE_SOURCE_DIR}/TPC/TPCrec)

# Include directories for CLING
get_directory_property(incdirs INCLUDE_DIRECTORIES)
foreach( d ${incdirs})    
  set(include_dirs ${include_dirs} -I${d})
endforeach()

# Sources
set ( SRCS   
  AliSegmentID.cxx 
  AliSegmentArray.cxx 
  AliDigits.cxx 
  AliH2F.cxx   # REMOVE
  AliTPCLoader.cxx 
  AliTPCPRF2D.cxx 
  AliTPCRF1D.cxx 
  AliDetectorParam.cxx 
  AliTPCParam.cxx 
  AliTPCParamSR.cxx 
  AliSimDigits.cxx 
  AliDigitsArray.cxx 
  AliTPCDigitsArray.cxx 
  AliTPCmapper.cxx 
  AliTPCROC.cxx 
  AliTPCCalROC.cxx 
  AliTPCCalPad.cxx 
  AliTPCcalibDB.cxx 
  AliTPCcalibDButil.cxx 
  AliTPCAltroMapping.cxx 
  AliTPCRawStreamV3.cxx 
  AliTPCSensorTemp.cxx 
  AliTPCSensorTempArray.cxx 
  AliTPCCalibRawBase.cxx 
  AliTPCCalibPedestal.cxx 
  AliTPCCalibPulser.cxx 
  AliTPCCalibCE.cxx 
  AliTPCCalibRaw.cxx 
  AliTPCPreprocessor.cxx 
  AliTPCPreprocessorOnline.cxx
  AliTPCCalibViewer.cxx        # UTIL
  AliTPCCalibViewerGUI.cxx     # UTIL
  AliTPCCalibViewerGUItime.cxx # UTIL
  AliTPCExB.cxx 
  AliTPCExBExact.cxx 
  AliTPCExBFirst.cxx 
  AliTPCTempMap.cxx 
  AliTPCCalibVdrift.cxx 
  AliTransform.cxx 
  AliTPCTransform.cxx 
  AliTPCdataQA.cxx 
  AliTPCQAChecker.cxx 
  AliTPCConfigDA.cxx 
  AliTPCConfigParser.cxx
  AliTPCPointCorrection.cxx
  AliTPCLaserTrack.cxx 
  AliTPCCorrection.cxx 
  AliTPCInverseCorrection.cxx 
  AliTPCComposedCorrection.cxx 
  AliTPCExBBShape.cxx 
  AliTPCExBTwist.cxx 
  AliTPCGGVoltError.cxx 
  AliTPCBoundaryVoltError.cxx 
  AliTPCCalibGlobalMisalignment.cxx 
  AliTPCFCVoltError3D.cxx 
  AliTPCROCVoltError3D.cxx 
  AliTPCSpaceCharge.cxx 
  AliTPCSpaceCharge3D.cxx 
  AliXRDPROOFtoolkit.cxx 
  AliTPCExBEffective.cxx 
  AliTPCExBEffectiveSector.cxx 
  AliTPCCorrectionDrift.cxx 
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
