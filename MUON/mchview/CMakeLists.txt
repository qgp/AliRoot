cmake_minimum_required (VERSION 2.8)

# Module
set (MODULE mchview)
include_directories(${CMAKE_SOURCE_DIR}/MUON/${MODULE})
include_directories( ${ROOTSYS}/include ${CMAKE_SOURCE_DIR}/STEER/STEERBase ${CMAKE_SOURCE_DIR}/STEER/CDB ${CMAKE_SOURCE_DIR}/MUON/MUONgraphics ${CMAKE_SOURCE_DIR}/MUON/MUONmapping ${CMAKE_SOURCE_DIR}/MUON/MUONcore)

# Standard Root libraries
execute_process(COMMAND root-config --libs OUTPUT_VARIABLE ROOTLIBS OUTPUT_STRIP_TRAILING_WHITESPACE)

# Additional Root libraries
execute_process(COMMAND root-config --libdir OUTPUT_VARIABLE ROOTLIBDIR OUTPUT_STRIP_TRAILING_WHITESPACE)
set(ROOTEXTRALIBS "-L${ROOTLIBDIR} -lXMLParser -lProof -lGui -lThread -lGeom -lVMC -lMinuit")

# Add an executable to the project using the specified source files
add_executable(${MODULE} mchview.cxx)
target_link_libraries(${MODULE} MUONmapping RAWDatabase MUONshuttle MUONgraphics MUONrec MUONgeometry MUONcalib MUONbase MUONraw RAWDatasim MUONtrigger MUONcore STEER CDB AOD RAWDatarec ESD RAWDatabase STEERBase ANALYSIS ${ROOTEXTRALIBS} ${ROOTLIBS})

# Installation
install(TARGETS ${MODULE} RUNTIME DESTINATION bin)
