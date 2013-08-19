cmake_minimum_required (VERSION 2.8)

# Module
set (MODULE AOD)
include_directories( ${ROOTSYS}/include ${CMAKE_SOURCE_DIR}/STEER/${MODULE})

# Additional include directories
include_directories( ${ROOTSYS}/include ${CMAKE_SOURCE_DIR}/STEER/STEERBase ${CMAKE_SOURCE_DIR}/STEER/ESD)

# Include directories for CLING
get_directory_property(incdirs INCLUDE_DIRECTORIES)
foreach( d ${incdirs})    
  set(include_dirs ${include_dirs} -I${d})
endforeach()

# Sources
set( SRCS  
  AliAODEvent.cxx 
  AliAODHeader.cxx 
  AliAODTrack.cxx 
  AliAODPid.cxx 
  AliAODVertex.cxx 
  AliAODCluster.cxx 
  AliAODCaloCluster.cxx 
  AliAODPmdCluster.cxx 
  AliAODFmdCluster.cxx 
  AliAODJet.cxx 
  AliAODJetEventBackground.cxx 
  AliAODPhoton.cxx 
  AliAODRedCov.cxx 
  AliAODRecoDecay.cxx 
  AliAODExtension.cxx 
  AliAODHandler.cxx 
  AliAODTracklets.cxx 
  AliAODTagCreator.cxx 
  AliAODv0.cxx 
  AliAODcascade.cxx 
  AliAODCaloCells.cxx 
  AliAODCaloTrigger.cxx 
  AliAODInputHandler.cxx 
  AliAODDiJet.cxx 
  AliAODMCParticle.cxx 
  AliAODMCHeader.cxx 
  AliAODPWG4Particle.cxx 
  AliAODPWG4ParticleCorrelation.cxx 
  AliAODDimuon.cxx 
  AliAODpidUtil.cxx 
  AliAODBranchReplicator.cxx 
  AliAODTZERO.cxx
  AliAODVZERO.cxx
  AliAODZDC.cxx	
  AliAODHMPIDrings.cxx	
  AliAODTrdTrack.cxx
  AliAODTrdTracklet.cxx
  )

# Sources from headers
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
