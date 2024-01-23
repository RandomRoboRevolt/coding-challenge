cmake_minimum_required(VERSION 3.5)
project(EmbeddedDevelopment)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
include(GenerateExportHeader)
include(CTest)

set(CMAKE_CXX_STANDARD 17)
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
endif(${BUILD_SHARED_LIBS})

# Find boost::sml, if not available, fetch from upstream
include(FetchContent)
message(STATUS "Checking for boost::sml, will fetch if not available")
FetchContent_Declare(
  sml
  GIT_REPOSITORY https://github.com/boost-ext/sml.git
  GIT_TAG        751b6f10b73c086e0e8850a7ef5b5aea2136831b #v1.1.9
  FIND_PACKAGE_ARGS
)
FetchContent_MakeAvailable(sml)
find_package(sml CONFIG REQUIRED)



add_subdirectory(motordriver)
add_subdirectory(master)
add_dependencies(master motordriver)
if (BUILD_TESTING)
  add_subdirectory(test-unit)
endif(BUILD_TESTING)
