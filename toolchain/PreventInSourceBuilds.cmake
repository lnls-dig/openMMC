# Adapted from ITK solution
# https://github.com/InsightSoftwareConsortium/ITK/blob/master/CMake/PreventInSourceBuilds.cmake

# This function will prevent in-source builds
function(AssureOutOfSourceBuilds)
  # make sure the user doesn't play dirty with symlinks
  get_filename_component(srcdir "${CMAKE_SOURCE_DIR}" REALPATH)
  get_filename_component(bindir "${CMAKE_BINARY_DIR}" REALPATH)

  # disallow in-source builds
  if("${srcdir}" STREQUAL "${bindir}")
    message("######################################################")
    message("# openMMC should not be configured & built in the root source directory")
    message("# You must run cmake in a build directory.")
    message("# For example:")
    message("# mkdir build")
    message("#")
    message("# Then you can proceed to configure and build")
    message("# by using the following commands")
    message("#")
    message("# cd build")
    message("# cmake <source_dir> <other_flags> && make ")
    message("#")
    message("# or:")
    message("# cmake -B<build_folder> -H<source_folder> <other_flags>")
    message("#")
    message("# NOTE: Given that you already tried to make an in-source build")
    message("#       CMake have already created several files & directories")
    message("#       in your source tree. Run the folowing commands to remove them:")
    message("#")
    message("#       cd <source-dir>")
    message("#       git clean -fdX -n")
    message("#       git clean -fdX")
    message("#")
    message("######################################################")
    message(FATAL_ERROR "Quitting configuration")
  endif()
endfunction()

AssureOutOfSourceBuilds()
