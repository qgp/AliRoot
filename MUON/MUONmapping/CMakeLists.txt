cmake_minimum_required (VERSION 2.8)

# Module
set (MODULE MUONmapping)
include_directories( ${ROOTSYS}/include ${CMAKE_SOURCE_DIR}/MUON/${MODULE})

# Additional include directories
include_directories( ${ROOTSYS}/include ${CMAKE_SOURCE_DIR}/STEER/STEERBase ${CMAKE_SOURCE_DIR}/MUON/MUONcore ${CMAKE_SOURCE_DIR}/STEER/CDB)

# Include directories for CLING
get_directory_property(incdirs INCLUDE_DIRECTORIES)
foreach( d ${incdirs})    
  set(include_dirs ${include_dirs} -I${d})
endforeach()

# Sources
# Category : basic

set ( SRCS ${SRCS}  
  AliMpPlaneType.cxx 
  AliMpCathodType.cxx 
  AliMpStationType.cxx 
  AliMpStation12Type.cxx 
  AliMpArea.cxx 
  AliMpConstants.cxx 
  AliMpFiles.cxx 
  AliMpPad.cxx 
  AliMpEncodePair.cxx 
  AliMpVIndexed.cxx 
  AliMpVPadIterator.cxx 
  AliMpVSegmentation.cxx 
  AliMpDataProcessor.cxx 
  AliMpDataStreams.cxx 
  AliMpDataMap.cxx
  )

# Category : motif

set ( SRCS ${SRCS}  
  AliMpConnection.cxx 
  AliMpMotif.cxx 
  AliMpMotifMap.cxx 
  AliMpMotifPosition.cxx 
  AliMpMotifPositionPadIterator.cxx 
  AliMpMotifReader.cxx 
  AliMpMotifSpecial.cxx 
  AliMpMotifType.cxx 
  AliMpMotifTypePadIterator.cxx 
  AliMpVMotif.cxx
  )

# Category : sector

set ( SRCS ${SRCS}  
  AliMpPadRow.cxx 
  AliMpPadRowLSegment.cxx 
  AliMpPadRowRSegment.cxx 
  AliMpRow.cxx 
  AliMpRowSegment.cxx 
  AliMpRowSegmentLSpecial.cxx 
  AliMpRowSegmentRSpecial.cxx 
  AliMpSectorAreaHPadIterator.cxx 
  AliMpSectorAreaVPadIterator.cxx 
  AliMpSector.cxx 
  AliMpSectorPadIterator.cxx 
  AliMpSectorReader.cxx 
  AliMpSectorSegmentation.cxx 
  AliMpSubZone.cxx 
  AliMpVPadRowSegment.cxx 
  AliMpVRowSegment.cxx 
  AliMpVRowSegmentSpecial.cxx 
  AliMpZone.cxx
  )

# Category : slat

set ( SRCS ${SRCS}  
  AliMpHelper.cxx 
  AliMpPCB.cxx 
  AliMpSlat.cxx 
  AliMpSlatPadIterator.cxx 
  AliMpSlatSegmentation.cxx 
  AliMpPCBPadIterator.cxx 
  AliMpSt345Reader.cxx 
  AliMpSlatMotifMap.cxx
  )

# Category : trigger

set ( SRCS ${SRCS}  
  AliMpTrigger.cxx 
  AliMpTriggerReader.cxx 
  AliMpTriggerSegmentation.cxx 
  AliMpRegionalTrigger.cxx 
  AliMpTriggerCrate.cxx 
  AliMpLocalBoard.cxx
  )

# Category : management

set ( SRCS ${SRCS}  
  AliMpManuIterator.cxx 
  AliMpManuUID.cxx 
  AliMpPadUID.cxx 
  AliMpHVUID.cxx 
  AliMpDCSNamer.cxx 
  AliMpSegmentation.cxx 
  AliMpDetElement.cxx 
  AliMpDEStore.cxx 
  AliMpDEIterator.cxx 
  AliMpDEManager.cxx 
  AliMpBusPatch.cxx 
  AliMpDDL.cxx 
  AliMpDDLStore.cxx 
  AliMpCDB.cxx 
  AliMpFrtCrocusConstants.cxx 
  AliMpManuStore.cxx 
  AliMpFastSegmentation.cxx 
  AliMpUID.cxx
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
