# **************************************************************************
# * Copyright(c) 1998-2014, ALICE Experiment at CERN, All rights reserved. *
# *                                                                        *
# * Author: The ALICE Off-line Project.                                    *
# * Contributors are mentioned in the code where appropriate.              *
# *                                                                        *
# * Permission to use, copy, modify and distribute this software and its   *
# * documentation strictly for non-commercial purposes is hereby granted   *
# * without fee, provided that the above copyright notice appears in all   *
# * copies and that both the copyright notice and this permission notice   *
# * appear in the supporting documentation. The authors make no claims     *
# * about the suitability of this software for any purpose. It is          *
# * provided "as is" without express or implied warranty.                  *
# **************************************************************************

# Module
set(MODULE STEERRec)

# Module include folders
include_directories(${AliRoot_SOURCE_DIR}/STEER/${MODULE})

# Additional include directories - alphabetical order except ROOT
include_directories(${ROOT_INCLUDE_DIR}
                    ${AliRoot_SOURCE_DIR}/ANALYSIS/ANALYSIS
                    ${AliRoot_SOURCE_DIR}/HLT/BASE
                    ${AliRoot_SOURCE_DIR}/RAW/RAWDatabase
                    ${AliRoot_SOURCE_DIR}/RAW/RAWDatarec
                    ${AliRoot_SOURCE_DIR}/STEER/CDB
                    ${AliRoot_SOURCE_DIR}/STEER/ESD
                    ${AliRoot_SOURCE_DIR}/STEER/STEER
                    ${AliRoot_SOURCE_DIR}/STEER/STEERBase
                    ${AliRoot_SOURCE_DIR}/ITS/ITSbase
                    ${AliRoot_SOURCE_DIR}/ITS/UPGRADE/ITSUpgradeBase
                    ${AliRoot_SOURCE_DIR}/ITS/UPGRADE/ITSUpgradeRec
                    ${AliRoot_SOURCE_DIR}/TPC/TPCrec
                    ${AliRoot_SOURCE_DIR}/TPC/TPCbase
                    ${AliRoot_SOURCE_DIR}/TRD/TRDbase
                    ${AliRoot_SOURCE_DIR}/TOF/TOFrec
                    ${AliRoot_SOURCE_DIR}/TOF/TOFbase
                    ${AliRoot_SOURCE_DIR}/PHOS/PHOSbase
                    ${AliRoot_SOURCE_DIR}/HMPID/HMPIDrec
                    ${AliRoot_SOURCE_DIR}/HMPID/HMPIDbase
                    ${AliRoot_SOURCE_DIR}/RAW/RAWDatasim
                    ${AliRoot_SOURCE_DIR}/EMCAL/EMCALrec
                    ${AliRoot_SOURCE_DIR}/EMCAL/EMCALbase
                    ${AliRoot_SOURCE_DIR}/EMCAL/EMCALUtils
                    ${AliRoot_SOURCE_DIR}/MUON/MUONrec
                    ${AliRoot_SOURCE_DIR}/MUON/MUONbase
                    ${AliRoot_SOURCE_DIR}/MUON/MUONcore
                    ${AliRoot_SOURCE_DIR}/FMD/FMDrec
                    ${AliRoot_SOURCE_DIR}/FMD/FMDbase
                    ${AliRoot_SOURCE_DIR}/ZDC/ZDCrec
                    ${AliRoot_SOURCE_DIR}/PMD/PMDrec
                    ${AliRoot_SOURCE_DIR}/PMD/PMDbase
                    ${AliRoot_SOURCE_DIR}/T0/T0rec
                    ${AliRoot_SOURCE_DIR}/T0/T0base
                    ${AliRoot_SOURCE_DIR}/VZERO/VZEROrec
                    ${AliRoot_SOURCE_DIR}/VZERO/VZERObase
                    ${AliRoot_SOURCE_DIR}/ACORDE/ACORDErec
                    ${AliRoot_SOURCE_DIR}/ACORDE/ACORDEbase
                    ${AliRoot_SOURCE_DIR}/AD/ADrec
                    ${AliRoot_SOURCE_DIR}/AD/ADbase
                    ${AliRoot_SOURCE_DIR}/FIT/FITrec
                    ${AliRoot_SOURCE_DIR}/MFT/MFTrec
                    ${AliRoot_SOURCE_DIR}/MFT/MFTbase
                    ${AliRoot_SOURCE_DIR}/MFT/MFTsim
                    ${AliRoot_SOURCE_DIR}/HLT/rec
                    ${CMAKE_BINARY_DIR}/version/
                   )

# Sources - alphabetical order
set(SRCS
    AliReconstruction.cxx
   )

# Headers from sources
string(REPLACE ".cxx" ".h" HDRS "${SRCS}")

# Generate the dictionary
# It will create G_ARG1.cxx and G_ARG1.h / ARG1 = function first argument
get_directory_property(incdirs INCLUDE_DIRECTORIES)
generate_dictionary("${MODULE}" "${MODULE}LinkDef.h" "${HDRS}" "${incdirs}")

set(ROOT_DEPENDENCIES Core EG GenVector GeomPainter Geom Gpad Graf Hist MathCore Matrix Minuit Net Physics Proof RIO Tree VMC)
set(ALIROOT_DEPENDENCIES STEER STEERBase RAWDatabase RAWDatarec CDB ESD ANALYSIS HLTbase ITSbase TPCrec TPCbase TRDbase TOFrec TOFbase PHOSbase HMPIDrec HMPIDbase RAWDatasim EMCALrec EMCALbase EMCALUtils MUONrec MUONbase MUONcore FMDrec FMDbase ZDCrec PMDrec PMDbase T0rec T0base VZEROrec VZERObase ACORDErec ACORDEbase ADrec ADsim ADbase FITrec MFTrec MFTbase MFTsim HLTrec ITSUpgradeRec)

# Generate the ROOT map
# Dependecies
set(LIBDEPS ${ALIROOT_DEPENDENCIES} ${ROOT_DEPENDENCIES})
generate_rootmap("${MODULE}" "${LIBDEPS}" "${CMAKE_CURRENT_SOURCE_DIR}/${MODULE}LinkDef.h")

set(LIBHLTSIM_VERSION "1")
add_definitions("-DLIBHLTSIM_VERSION=${LIBHLTSIM_VERSION}")

# Create an object to be reused in case of static libraries 
# Otherwise the sources will be compiled twice
add_library(${MODULE}-object OBJECT ${SRCS} G__${MODULE}.cxx)
# Add a library to the project using the object
add_library(${MODULE} SHARED $<TARGET_OBJECTS:${MODULE}-object>)

target_link_libraries(${MODULE} ${ALIROOT_DEPENDENCIES} ${ROOT_DEPENDENCIES})

# Setting the correct headers for the object as gathered from the dependencies
target_include_directories(${MODULE}-object PUBLIC $<TARGET_PROPERTY:${MODULE},INCLUDE_DIRECTORIES>)
set_target_properties(${MODULE}-object PROPERTIES COMPILE_DEFINITIONS $<TARGET_PROPERTY:${MODULE},COMPILE_DEFINITIONS>)

# Public include folders that will be propagated to the dependecies
target_include_directories(${MODULE} PUBLIC ${incdirs})

# Additional compilation flags
set_target_properties(${MODULE}-object PROPERTIES COMPILE_FLAGS "")

# System dependent: Modify the way the library is build
if(${CMAKE_SYSTEM} MATCHES Darwin)
    set_target_properties(${MODULE} PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
endif(${CMAKE_SYSTEM} MATCHES Darwin)

# Installation
install(TARGETS ${MODULE}
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib)
install(FILES ${HDRS} DESTINATION include)

# Static version if DA enabled
if(ALIROOT_STATIC)
    add_library(${MODULE}-static STATIC $<TARGET_OBJECTS:${MODULE}-object>)
    set_target_properties(${MODULE}-static PROPERTIES OUTPUT_NAME ${MODULE})
    
    # list of shared dependencies / the name of the variable containing the list of static ones
    generate_static_dependencies("${ALIROOT_DEPENDENCIES}" "STATIC_ALIROOT_DEPENDENCIES")
    target_link_libraries(${MODULE}-static ${STATIC_ALIROOT_DEPENDENCIES} Root RootExtra)

    # Public include folders that will be propagated to the dependecies
    target_include_directories(${MODULE}-static PUBLIC ${incdirs})

    set_target_properties(${MODULE}-static PROPERTIES LINK_FLAGS "-Wl,--whole-archive")

    # Installation
    install(TARGETS ${MODULE}-static
            ARCHIVE DESTINATION lib
            LIBRARY DESTINATION lib)
endif(ALIROOT_STATIC)

