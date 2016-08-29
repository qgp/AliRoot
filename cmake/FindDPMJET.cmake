# Find module for DPMJET.
# Variables:
#  - DPMJET_FOUND: TRUE if found
#  - DPMJET_LIBPATH: library path
include(FindPackageHandleStandardArgs)
find_library(DPMJET_LIBPATH DPMJET
             PATHS "${DPMJET}/lib" NO_DEFAULT_PATH)
get_filename_component(DPMJET_LIBPATH ${DPMJET_LIBPATH} DIRECTORY)
find_package_handle_standard_args(DPMJET DEFAULT_MSG
                                  DPMJET_LIBPATH)
