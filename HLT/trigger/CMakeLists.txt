cmake_minimum_required (VERSION 2.8)

include ("${CMAKE_SOURCE_DIR}/HLT/CMakehlt.conf")

# Module
set (MODULE AliHLTTrigger)
include_directories( ${ROOTSYS}/include ${CMAKE_SOURCE_DIR}/HLT/trigger)

# Additional include directories
include_directories( 
  ${CMAKE_SOURCE_DIR}/STEER/STEERBase ${CMAKE_SOURCE_DIR}/STEER/ESD ${CMAKE_SOURCE_DIR}/STEER/AOD ${CMAKE_SOURCE_DIR}/STEER/CDB ${CMAKE_SOURCE_DIR}/STEER/STEER
  ${CMAKE_SOURCE_DIR}/HLT/BASE ${CMAKE_SOURCE_DIR}/HLT/BASE/util 
  ${CMAKE_SOURCE_DIR}/HLT/ITS ${CMAKE_SOURCE_DIR}/HLT/TRD ${CMAKE_SOURCE_DIR}/HLT/MUON
  ${CMAKE_SOURCE_DIR}/TRD/TRDbase ${CMAKE_SOURCE_DIR}/TRD/TRDrec
  ${CMAKE_SOURCE_DIR}/MUON/MUONraw
  ${CMAKE_SOURCE_DIR}/TPC/TPCcalib
  ${CMAKE_SOURCE_DIR}/RAW/RAWDatabase
  ${CMAKE_SOURCE_DIR}/ANALYSIS/ANALYSISalice)# ${CMAKE_SOURCE_DIR}/RAW/RAWDatarec ${CMAKE_SOURCE_DIR}/HLT/BASE/HOMER)

# Include directories for CLING
get_directory_property(incdirs INCLUDE_DIRECTORIES)
foreach( d ${incdirs})    
  set(include_dirs ${include_dirs} -I${d})
endforeach()

# Sources
set ( SRCS 
    AliHLTTrigger.cxx
    AliHLTGlobalTrigger.cxx
    AliHLTGlobalTriggerWrapper.cxx
    AliHLTGlobalTriggerConfig.cxx
    AliHLTGlobalTriggerComponent.cxx
    AliHLTTriggerBarrelMultiplicity.cxx
    AliHLTD0Trigger.cxx
    AliHLTD0toKpi.cxx
    AliHLTD0Candidate.cxx
    AliHLTTriggerITSMultiplicity.cxx
    AliHLTTriggerAgent.cxx
    AliHLTTriggerCaloClusterEnergy.cxx
    AliHLTTriggerPhosClusterEnergy.cxx
    AliHLTTriggerEmcalClusterEnergy.cxx
    AliHLTTriggerPhosMip.cxx
    AliHLTTriggerTrdClusterMultiplicity.cxx
    AliHLTTriggerBarrelGeomMultiplicity.cxx
    AliHLTTriggerDetectorGeom.cxx
    AliHLTTriggerDetectorGeomRectangle.cxx
    AliHLTTriggerDecisionParameters.cxx
    AliHLTTriggerGammaConversion.cxx
    AliHLTMuonSpectroScalars.cxx
    AliHLTMuonSpectroTriggerComponent.cxx
    AliHLTUpcTriggerComponent.cxx
    AliHLTTriggerCosmics.cxx
    AliHLTTriggerCounterComponent.cxx
    AliHLTTriggerCounters.cxx
    AliHLTTriggerCaloClusterEnergy.cxx
    AliHLTTriggerEmcalElectron.cxx	
   AliHLTTRDTriggerComponent.cxx
# uncomment if fastjet is installed
#    AliHLTTriggerFastJet.cxx
# ---------------------------------
    AliHLTFastJetMonitorComponent.cxx
    AliHLTFastJetMonitor.cxx
    AliHLTEmcalElectronMonitor.cxx
    AliHLTEmcalElectronMonitorComponent.cxx
  )

# Headers from sources
string(REPLACE ".cxx" ".h" HDRS "${SRCS}")

# Definitions
add_definitions("${HLTDEFS}")

# Definitions for CLING
get_directory_property(defs DEFINITIONS)
separate_arguments(defs)
foreach( dd ${defs})    
  set(CLINGDEFS ${CLINGDEFS} ${dd})
endforeach()

# Make dictionary
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/G__${MODULE}.cxx ${CMAKE_CURRENT_BINARY_DIR}/G__${MODULE}.h
  COMMAND ${ROOTCINT} 
  ARGS -f ${CMAKE_CURRENT_BINARY_DIR}/G__${MODULE}.cxx -c -p 
  ${include_dirs}
  ${CLINGDEFS}
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
