cmake_minimum_required (VERSION 2.8)

# Module
set (MODULE TenderSupplies)
include_directories( ${ROOTSYS}/include ${CMAKE_SOURCE_DIR}/ANALYSIS/${MODULE})

# Additional include directories
include_directories( ${ROOTSYS}/include ${CMAKE_SOURCE_DIR}/STEER/STEERBase ${CMAKE_SOURCE_DIR}/STEER/STEER ${CMAKE_SOURCE_DIR}/STEER/CDB ${CMAKE_SOURCE_DIR}/STEER/ESD ${CMAKE_SOURCE_DIR}/STEER/AOD ${CMAKE_SOURCE_DIR}/OADB ${CMAKE_SOURCE_DIR}/ANALYSIS/ANALYSIS ${CMAKE_SOURCE_DIR}/ANALYSIS/ANALYSISalice ${CMAKE_SOURCE_DIR}/ANALYSIS/Tender ${CMAKE_SOURCE_DIR}/TOF/TOFbase ${CMAKE_SOURCE_DIR}/TOF/TOFrec ${CMAKE_SOURCE_DIR}/T0/T0rec ${CMAKE_SOURCE_DIR}/TPC/TPCbase ${CMAKE_SOURCE_DIR}/TRD/TRDbase ${CMAKE_SOURCE_DIR}/VZERO/VZERObase ${CMAKE_SOURCE_DIR}/VZERO/VZEROrec ${CMAKE_SOURCE_DIR}/EMCAL/EMCALrec ${CMAKE_SOURCE_DIR}/EMCAL/EMCALbase ${CMAKE_SOURCE_DIR}/EMCAL/EMCALUtils ${CMAKE_SOURCE_DIR}/PHOS/PHOSbase ${CMAKE_SOURCE_DIR}/PHOS/PHOSUtils ${CMAKE_SOURCE_DIR}/PHOS/PHOSrec)

# Include directories for CLING
get_directory_property(incdirs INCLUDE_DIRECTORIES)
foreach( d ${incdirs})    
  set(include_dirs ${include_dirs} -I${d})
endforeach()

# Sources
set ( SRCS  
  AliTOFTenderSupply.cxx 
  AliPIDTenderSupply.cxx   
  AliTPCTenderSupply.cxx 
  AliTRDTenderSupply.cxx 
  AliVtxTenderSupply.cxx 
  AliVZEROTenderSupply.cxx 
  AliEMCALTenderSupply.cxx 
  AliPHOSTenderSupply.cxx
  AliHMPIDTenderSupply.cxx
  AliT0TenderSupply.cxx 
  AliTrackFixTenderSupply.cxx 
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
