cmake_minimum_required (VERSION 2.8)

# Module
set (MODULE RAWDatabase)
include_directories( ${ROOTSYS}/include ${CMAKE_SOURCE_DIR}/RAW/${MODULE})

# Additional include directories
include_directories( ${ROOTSYS}/include ${CMAKE_SOURCE_DIR}/STEER/STEERBase)

# Include directories for CLING
get_directory_property(incdirs INCLUDE_DIRECTORIES)
foreach( d ${incdirs})    
  set(include_dirs ${include_dirs} -I${d})
endforeach()

# Sources
set( SRCS   
  AliRawEventHeaderBase.cxx 
  AliRawEquipmentHeader.cxx 
  AliRawEquipment.cxx 
  AliRawEvent.cxx 
  AliRawData.cxx 
  AliRawVEquipment.cxx 
  AliRawEquipmentV2.cxx 
  AliRawVEvent.cxx 
  AliRawEventV2.cxx 
  AliRawDataArrayV2.cxx 
  AliRawDataArray.cxx 
  AliStats.cxx 
  AliRawEventTag.cxx 
  AliAltroMapping.cxx 
  AliCaloAltroMapping.cxx 
  )

# Headers from sources
string(REPLACE ".cxx" ".h" HDRS "${SRCS}" )

# Additional header files with no implementation
set(HDRS ${HDRS} AliRawEventHeaderVersions.h AliRawDataHeader.h)

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
  COMPILE_FLAGS "-Wall -Weffc++"
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
