# This CMake function generates a target that, in turn, will generate a PARfile for the given
# library.
#
# Usage: in the CMakeLists.txt, for a given library, add the following:
#   add_target_parfile(${MODULE} "${SRCS}" "${HDRS}" "${MODULE}LinkDef.h" "${LIBDEPS}" ["extrainclude1 extrainclude2..."])
#
# Arguments are, in order:
#  - library's name: for libBLAHBLAH it will generate a target BLAHBLAH.par
#  - source files: classes to include in the PARfile, they must be exactly the ones used to generate
#    the library
#  - headers
#  - the LinkDef used by ROOT
#  - dependent libraries: used to generate the rootmap
#  - extra include paths (optional): passed during compilation
#
# To generate a PARfile, if enabled in its CMakeLists.txt, go to the build directory and run:
#   make BLAHBLAH.par

function(add_target_parfile PARMODULE PARSOURCES PARHEADERS PARLINKDEF PARLIBDEPS)

  # Libraries: result is a space-separated string
  foreach(_THISLIB ${PARLIBDEPS})
    set(_PARLIBDEPS "${_PARLIBDEPS} lib${_THISLIB}")
  endforeach()
  string(STRIP "${_PARLIBDEPS}" PARLIBDEPS)

  # Export variables: used in configure_file()
  set(PARMODULE "${PARMODULE}")
  string(REPLACE ";" " " PARSOURCES_FLAT "${PARSOURCES}")
  string(REPLACE ";" " " PARHEADERS_FLAT "${PARHEADERS}")

  #message(STATUS "[add_target_parfile] Library (space-separated): ${PARMODULE}")
  #message(STATUS "[add_target_parfile] Sources (list): ${PARSOURCES}")
  #message(STATUS "[add_target_parfile] Headers (list): ${PARHEADERS}")
  #message(STATUS "[add_target_parfile] Dependencies (space-separated): ${PARLIBDEPS}")

  if(NOT "${ARGV5}" STREQUAL "")
    # Optional: extra includes, space-separated
    set(PAREXTRAINCLUDES "${ARGV5}")
    #message(STATUS "[add_target_parfile] Extra Includes (space-separated): ${PAREXTRAINCLUDES}")
  endif()

  # PARfile temporary directory
  set(PARTMP ${CMAKE_CURRENT_BINARY_DIR}/PARfiles)

  # PARfile output directory (the one we will tar)
  set(PARDIR ${PARTMP}/${PARMODULE})

  # PARfile meta directory
  set(PARMETADIR ${PARTMP}/${PARMODULE}/PROOF-INF)

  # Destination PARfile (full path)
  set(PARFILE ${CMAKE_CURRENT_BINARY_DIR}/${PARMODULE}.par)

  # Create Makefile
  configure_file(
      ${PROJECT_SOURCE_DIR}/cmake/PARfiles/Makefile.in
      ${PARTMP}/Makefile
      @ONLY
  )

  # Create BUILD.sh
  configure_file(
      ${PROJECT_SOURCE_DIR}/cmake/PARfiles/BUILD.sh.in
      ${PARTMP}/BUILD.sh
      @ONLY
  )
  execute_process(COMMAND chmod a+x ${PARTMP}/BUILD.sh)

  # Create SETUP.C
  configure_file(
      ${PROJECT_SOURCE_DIR}/cmake/PARfiles/SETUP.C.in
      ${PARTMP}/SETUP.C
      @ONLY
  )

  # Target for creating PARfile (would stop after the first failed COMMAND)
  add_custom_target("${PARMODULE}.par"

    COMMAND ${CMAKE_COMMAND} -E remove_directory ${PARDIR}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${PARDIR}

    # TODO: cmake -E copy unfortunately does not handle multiple files
    COMMAND rsync --relative ${PARSOURCES} ${PARHEADERS} ${PARLINKDEF} ${PARDIR}/

    COMMAND ${CMAKE_COMMAND} -E copy ${PARTMP}/Makefile ${PARDIR}/Makefile
    COMMAND ${CMAKE_COMMAND} -E copy ${PARTMP}/SETUP.C ${PARMETADIR}/SETUP.C
    COMMAND ${CMAKE_COMMAND} -E copy ${PARTMP}/BUILD.sh ${PARMETADIR}/BUILD.sh

    COMMAND ${CMAKE_COMMAND} -E chdir ${PARTMP}
            ${CMAKE_COMMAND} -E tar czf ${PARFILE} ${PARMODULE}/

    COMMAND ${CMAKE_COMMAND} -E remove_directory ${PARDIR}

    COMMENT "Building PAR file ${PARMODULE}"
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  )

  # Install target
  install(FILES ${PARFILE} DESTINATION PARfiles OPTIONAL)

  # Add this module to the list of generated PARfiles
  list(APPEND ALIPARFILES ${PARMODULE})
  set(ALIPARFILES ${ALIPARFILES} CACHE INTERNAL "ALIPARFILES")

endfunction()
