cmake_minimum_required (VERSION 2.8)

# Module
set (MODULE MUONgraphics)
include_directories( ${ROOTSYS}/include ${CMAKE_SOURCE_DIR}/MUON/${MODULE})

# Additional include directories
include_directories( ${ROOTSYS}/include ${CMAKE_SOURCE_DIR}/STEER/STEERBase ${CMAKE_SOURCE_DIR}/STEER/CDB ${CMAKE_SOURCE_DIR}/STEER/ESD ${CMAKE_SOURCE_DIR}/STEER/STEER ${CMAKE_SOURCE_DIR}/MUON/MUONmapping ${CMAKE_SOURCE_DIR}/MUON/MUONrec ${CMAKE_SOURCE_DIR}/MUON/MUONcore ${CMAKE_SOURCE_DIR}/MUON/MUONcalib ${CMAKE_SOURCE_DIR}/MUON/MUONgeometry ${CMAKE_SOURCE_DIR}/MUON/MUONbase ${CMAKE_SOURCE_DIR}/RAW/RAWDatarec ${CMAKE_SOURCE_DIR}/RAW/RAWDatabase)

# Include directories for CLING
get_directory_property(incdirs INCLUDE_DIRECTORIES)
foreach( d ${incdirs})    
  set(include_dirs ${include_dirs} -I${d})
endforeach()

# Sources
set ( SRCS   
    AliMUONVPainter.cxx 
    AliMUONPCBPainter.cxx 
    AliMUONDEPainter.cxx 
    AliMUONPainterHelper.cxx 
    AliMUONChamberPainter.cxx 
    AliMUONManuPainter.cxx 
    AliMUONPainterPadStore.cxx 
    AliMUONPainterMasterFrame.cxx 
    AliMUONPainterMatrixFrame.cxx 
    AliMUONManuPadPainter.cxx 
    AliMUONPainterGroup.cxx 
    AliMUONPainterMatrix.cxx 
    AliMUONAttPainter.cxx 
    AliMUONPainterRegistry.cxx 
    AliMUONPainterDataRegistry.cxx 
    AliMUONPainterHighlighter.cxx 
    AliMUONPainterPlotSelector.cxx 
    AliMUONPainterInterfaceHelper.cxx 
    AliMUONBusPatchPainter.cxx 
    AliMUONPainterColorSlider.cxx 
    AliMUONAttPainterSelectorFrame.cxx 
    AliMUONPainterDataSourceFrame.cxx 
    AliMUONPainterDataSourceItem.cxx 
    AliMUONPainterEnv.cxx 
    AliMUONMchViewApplication.cxx 
    AliMUONTrackerDataHistogrammer.cxx 
    AliMUONTrackerDataWrapper.cxx 
    AliMUONTrackerDataCompareDialog.cxx 
    AliMUONContourPainter.cxx 
    AliMUONPolygon.cxx 
    AliMUONContour.cxx 
    AliMUONContourMaker.cxx 
    AliMUONManuContourMaker.cxx 
    AliMUONSegment.cxx 
    AliMUONSegmentTree.cxx 
    AliMUONNode.cxx 
    AliMUONPointWithRef.cxx 
    AliMUONContourMakerTest.cxx 
    AliMUONContourHandler.cxx
    AliMUONAlignmentCompareDialog.cxx
    AliMUONTrackerHV.cxx
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
install(TARGETS ${MODULE}
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib)
install(FILES ${HDRS} DESTINATION include)
