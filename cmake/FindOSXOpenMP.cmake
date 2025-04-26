# Adapted from https://gitlab.kitware.com/cmake/cmake/issues/18520 and
# https://cliutils.gitlab.io/modern-cmake/chapters/packages/OpenMP.html
# The built-in FindOpenMP can't seem to find OpenMP on OSX, so we use this custom
# module (which only works for CXX)

execute_process(COMMAND brew --prefix libomp
  OUTPUT_VARIABLE HOMEBREW_LIBOMP_PREFIX
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

find_library(OpenMP_LIBRARY
  NAMES omp
  HINTS "${HOMEBREW_LIBOMP_PREFIX}/lib"
)

find_path(OpenMP_INCLUDE_DIR
  omp.h
  HINTS "${HOMEBREW_LIBOMP_PREFIX}/include"
)

mark_as_advanced(OpenMP_LIBRARY OpenMP_INCLUDE_DIR)

include(FindPackageHandleStandardArgs)
set(FPHSA_NAME_MISMATCHED TRUE)
find_package_handle_standard_args(OpenMP DEFAULT_MSG OpenMP_LIBRARY OpenMP_INCLUDE_DIR)

if (OpenMP_FOUND)
  find_package(Threads REQUIRED)

  set(OpenMP_CXX_FOUND true)
  set(OpenMP_CXX_LIBRARIES ${OpenMP_LIBRARY})
  set(OpenMP_CXX_INCLUDE_DIRS ${OpenMP_INCLUDE_DIR})
  set(OpenMP_CXX_FLAGS -Xpreprocessor -fopenmp)

  add_library(OpenMP::OpenMP_CXX IMPORTED INTERFACE)
  set_target_properties(OpenMP::OpenMP_CXX PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${OpenMP_CXX_INCLUDE_DIRS}"
    INTERFACE_COMPILE_OPTIONS "${OpenMP_CXX_FLAGS}"
  )
  set_property(TARGET OpenMP::OpenMP_CXX
    PROPERTY INTERFACE_LINK_LIBRARIES "${OpenMP_CXX_FLAGS}" "${OpenMP_CXX_LIBRARIES}" Threads::Threads
  )
endif()
