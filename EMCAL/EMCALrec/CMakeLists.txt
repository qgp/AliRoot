cmake_minimum_required (VERSION 2.8)

# Module
set (MODULE EMCALrec)
include_directories( ${ROOTSYS}/include ${CMAKE_SOURCE_DIR}/EMCAL/${MODULE})

# Additional include directories
include_directories( ${ROOTSYS}/include ${CMAKE_SOURCE_DIR}/STEER/STEERBase ${CMAKE_SOURCE_DIR}/STEER/ESD ${CMAKE_SOURCE_DIR}/STEER/AOD ${CMAKE_SOURCE_DIR}/STEER/CDB ${CMAKE_SOURCE_DIR}/STEER/STEER ${CMAKE_SOURCE_DIR}/EMCAL/EMCALbase ${CMAKE_SOURCE_DIR}/EMCAL/EMCALUtils ${CMAKE_SOURCE_DIR}/EMCAL/EMCALsim ${CMAKE_SOURCE_DIR}/EMCAL/EMCALraw ${CMAKE_SOURCE_DIR}/RAW/RAWDatarec ${CMAKE_SOURCE_DIR}/RAW/RAWDatabase)# ${CMAKE_SOURCE_DIR}/RAW/RAWDatasim)

# Include directories for CLING
get_directory_property(incdirs INCLUDE_DIRECTORIES)
foreach( d ${incdirs})    
  set(include_dirs ${include_dirs} -I${d})
endforeach()

# Sources
set ( SRCS  
    AliEMCALReconstructor.cxx 
    AliEMCALClusterizer.cxx 
    AliEMCALClusterizerv1.cxx 
    AliEMCALClusterizerv2.cxx 
    AliEMCALClusterizerNxN.cxx
    AliEMCALClusterizerFixedWindow.cxx
    AliEMCALUnfolding.cxx 
    AliEMCALTracker.cxx 
    AliEMCALPID.cxx 
    AliEMCALQADataMakerRec.cxx 
    AliEMCALAfterBurnerUF.cxx 
    )

# Headers from sources
string(REPLACE ".cxx" ".h" HDRS "${SRCS}")

# Additional header files with no implementation
#set(HDRS ${HDRS} AliEMCALTriggerTypes.h)

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
