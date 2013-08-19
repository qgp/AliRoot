cmake_minimum_required (VERSION 2.8)

# Module
set (MODULE TOFbase)
include_directories( ${ROOTSYS}/include ${CMAKE_SOURCE_DIR}/TOF/${MODULE})

# Additional include directories
include_directories( ${ROOTSYS}/include ${CMAKE_SOURCE_DIR}/STEER/STEERBase ${CMAKE_SOURCE_DIR}/STEER/STEER ${CMAKE_SOURCE_DIR}/STEER/CDB ${CMAKE_SOURCE_DIR}/RAW/RAWDatarec ${CMAKE_SOURCE_DIR}/RAW/RAWDatabase ${CMAKE_SOURCE_DIR}/RAW/RAWDatasim ${CMAKE_SOURCE_DIR}/STEER/ESD ${CMAKE_SOURCE_DIR}/TOF/TOFsim ${CMAKE_SOURCE_DIR}/TOF/TOFrec)

# Include directories for CLING
get_directory_property(incdirs INCLUDE_DIRECTORIES)
foreach( d ${incdirs})    
  set(include_dirs ${include_dirs} -I${d})
endforeach()

# Sources
set ( SRCS  
    AliTOFGeometry.cxx 
    AliTOFdigit.cxx 
    AliTOFDigitMap.cxx
    AliTOFRawMap.cxx 
    AliTOFrawData.cxx 
    AliTOFRawStream.cxx 
    AliTOFHitData.cxx 
    AliTOFHitField.cxx 
    AliTOFHitDataBuffer.cxx 
    AliTOFDecoder.cxx
    AliTOFChannelOffline.cxx 
    AliTOFChannelOnline.cxx 
    AliTOFChannelOnlineStatus.cxx 
    AliTOFChannelOnlineArray.cxx 
    AliTOFChannelOnlineStatusArray.cxx 
    AliTOFcalib.cxx 
    AliTOFAlignment.cxx 
    AliTOFPreprocessor.cxx 
    AliTOFDataDCS.cxx
    AliTOFFormatDCS.cxx 
    AliTOFPreprocessorFDR.cxx 
    AliTOFFEEReader.cxx 
    AliTOFFEEDump.cxx 
    AliTOFCableLengthMap.cxx 
    AliTOFNoiseConfigHandler.cxx 
    AliTOFcalibHisto.cxx 
    AliTOFArray.cxx 
    AliTOFDaConfigHandler.cxx 
    AliTOFDeltaBCOffset.cxx 
    AliTOFCTPLatency.cxx 
    AliTOFT0Fill.cxx 
    AliTOFRunParams.cxx 
    AliTOFResponseParams.cxx 
    AliTOFT0FillOnlineCalib.cxx 
    AliTOFLvHvDataPoints.cxx 
    AliTOFDCSmaps.cxx 
    AliTOFChainSummaryData.cxx 
    AliTOFDRMSummaryData.cxx 
    AliTOFDecoderSummaryData.cxx 
    AliTOFLTMSummaryData.cxx 
    AliTOFTRMSummaryData.cxx 
    AliTOFDecoderV2.cxx 
    AliTOFTDCError.cxx 
    AliTOFTDCErrorBuffer.cxx 
    AliTOFTDCHit.cxx 
    AliTOFTDCHitBuffer.cxx 
    AliTOFReadoutInfo.cxx 
    )

# Headers from sources
string(REPLACE ".cxx" ".h" HDRS "${SRCS}")

# Additional header files with no implementation
set(HDRS ${HDRS} AliTOFRawDataFormat.h AliTOFDRMGlobalHeader.h AliTOFDRMGlobalTrailer.h AliTOFDRMStatusHeader1.h AliTOFDRMStatusHeader2.h AliTOFDRMStatusHeader3.h AliTOFDRMStatusHeader4.h AliTOFDRMEventCRC.h AliTOFLTMGlobalHeader.h AliTOFLTMGlobalTrailer.h AliTOFLTMPDLData.h AliTOFLTMADCData.h AliTOFLTMORData.h AliTOFTRMGlobalHeader.h AliTOFTRMGlobalTrailer.h AliTOFTRMChainHeader.h AliTOFTRMChainTrailer.h AliTOFTRMTDCError.h AliTOFTRMDiagnosticErrorWord1.h AliTOFTRMDiagnosticErrorWord2.h AliTOFFEEConfig.h AliTOFFEElightConfig.h)
#AliTOFTDCPackedHit.h 
#AliTOFTDCUnpackedHit.h 

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
