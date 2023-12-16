cmake_minimum_required(VERSION 3.5)
project(EmbeddedDevelopment)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_SHARED_LIBS "When true, shared libs are build, else static" ON)

## This is a little awkward stuff that you would usually hide away in some include
## basically, what it does, it sets the RPATH in such a way that your package is
## relocatable, i.e. even after you `cmake install`ed it, you can move the location
## of the installed files around and will still work.
if(${BUILD_SHARED_LIBS})
  set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
  # the RPATH to be used when installing, but only if it's not a system directory
  list(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_LIBDIR}" isSystemDir)
  if("${isSystemDir}" STREQUAL "-1")
    set(CMAKE_INSTALL_RPATH "$ORIGIN/../${CMAKE_INSTALL_LIBDIR}")
  endif("${isSystemDir}" STREQUAL "-1")
endif()

add_subdirectory(motordriver)
add_subdirectory(master)
