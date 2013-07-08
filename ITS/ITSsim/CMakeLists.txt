cmake_minimum_required (VERSION 2.8)

# Module
set (MODULE ITSsim)
include_directories( ${ROOTSYS}/include ${CMAKE_SOURCE_DIR}/ITS/${MODULE})

# Additional include directories
include_directories( ${ROOTSYS}/include ${CMAKE_SOURCE_DIR}/STEER/STEERBase ${CMAKE_SOURCE_DIR}/STEER/STEER ${CMAKE_SOURCE_DIR}/STEER/ESD ${CMAKE_SOURCE_DIR}/RAW/RAWDatarec ${CMAKE_SOURCE_DIR}/RAW/RAWDatabase ${CMAKE_SOURCE_DIR}/STEER/CDB ${CMAKE_SOURCE_DIR}/ITS/ITSbase ${CMAKE_SOURCE_DIR}/ITS/ITSrec)

# Include directories for CLING
get_directory_property(incdirs INCLUDE_DIRECTORIES)
foreach( d ${incdirs})    
  set(include_dirs ${include_dirs} -I${d})
endforeach()

# Sources
set ( SRCS 
  AliITS.cxx 
  AliITShit.cxx 
  AliITSSimuParam.cxx 
  AliITSDetTypeSim.cxx 
  AliITSmodule.cxx 
  AliITSsimulationFastPoints.cxx 
  AliITSsimulation.cxx 
  AliITSsimulationSPD.cxx 
  AliITSsimulationSDD.cxx 
  AliITSsimulationSSD.cxx 
  AliITSTableSSD.cxx 
  AliITSetfSDD.cxx  
  AliITSsDigitize.cxx 
  AliITSDigitizer.cxx 
  AliITSFOEfficiencySPD.cxx 
  AliITSFOEfficiencySPDColumn.cxx 
  AliITSFONoiseSPD.cxx 
  AliITSFOGeneratorSPD.cxx 
  AliITSFOSignalsSPD.cxx 
  AliITSTrigger.cxx 
  AliITSTriggerFOProcessor.cxx 
  AliITSv11.cxx 
  AliITSv11Geometry.cxx 
  AliITSv11GeomCable.cxx 
  AliITSv11GeomCableFlat.cxx 
  AliITSv11GeomCableRound.cxx 
  AliITSv11GeometrySDD.cxx 
  AliITSv11GeometrySSD.cxx 
  AliITSv11GeometrySPD.cxx 
  AliITSv11GeometrySupport.cxx 
  AliITSQADataMakerSim.cxx 
  AliITSQASPDDataMakerSim.cxx 
  AliITSQASDDDataMakerSim.cxx 
  AliITSQASSDDataMakerSim.cxx 
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
