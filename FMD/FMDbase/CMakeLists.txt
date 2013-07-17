cmake_minimum_required (VERSION 2.8)

# Module
set (MODULE FMDbase)
include_directories( ${ROOTSYS}/include ${CMAKE_SOURCE_DIR}/FMD/${MODULE})

# Additional include directories
include_directories( ${ROOTSYS}/include ${CMAKE_SOURCE_DIR}/STEER/STEERBase ${CMAKE_SOURCE_DIR}/STEER/STEER ${CMAKE_SOURCE_DIR}/STEER/ESD ${CMAKE_SOURCE_DIR}/STEER/CDB ${CMAKE_SOURCE_DIR}/RAW/RAWDatabase ${CMAKE_SOURCE_DIR}/FMD/FMDrec)

# Include directories for CLING
get_directory_property(incdirs INCLUDE_DIRECTORIES)
foreach( d ${incdirs})    
  set(include_dirs ${include_dirs} -I${d})
endforeach()

# Sources
set ( SRCS   
  AliFMDIndex.cxx 
  AliFMDBaseDigit.cxx 
  AliFMDDigit.cxx 
  AliFMDSDigit.cxx 
  AliFMDBoolMap.cxx 
  AliFMDUShortMap.cxx 
  AliFMDCalibPedestal.cxx 
  AliFMDCalibGain.cxx 
  AliFMDCalibSampleRate.cxx 
  AliFMDCalibStripRange.cxx 
  AliFMDAltroMapping.cxx 
  AliFMDParameters.cxx 
  AliFMDGeometry.cxx 
  AliFMDRing.cxx 
  AliFMDDetector.cxx 
  AliFMD1.cxx 
  AliFMD2.cxx 
  AliFMD3.cxx 
  AliFMDPreprocessor.cxx 
  AliFMDQAChecker.cxx 
  AliFMDGeometryBuilder.cxx 
  AliFMDSurveyToAlignObjs.cxx 
  )

# Headers from sources
string(REPLACE ".cxx" ".h" HDRS "${SRCS}")

# Additional header files with no implementation
set(HDRS ${HDRS} AliFMDCalibFwd.h AliFMDStripIndex.h AliFMDDebug.h)

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
