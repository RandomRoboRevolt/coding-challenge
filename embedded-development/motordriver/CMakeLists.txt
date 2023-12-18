## adding target without sources yet
add_library(motordriver "")

## Setting the version. This is a bit of a hack, because you probably rather deduct the
## version string from a git tag or the likes. But to keep it "simple" we just define it
## here. Could be exported to a config file so it can be used in the source code to 
## for example print out the version string. 
set(motordriver_VERSION_MAJOR "1")
set(motordriver_VERSION_MINOR "0")
set(motordriver_VERSION_PATCH "0")
set(motordriver_VERSION "${motordriver_VERSION_MAJOR}.${motordriver_VERSION_MINOR}.${motordriver_VERSION_PATCH}")

## This will, amogst other, ensure, we get proper versioned .so file
## and a symlink.
set_property(TARGET motordriver PROPERTY VERSION ${motordriver_VERSION})
set_property(TARGET motordriver PROPERTY SOVERSION ${motordriver_VERSION})
set_property(TARGET motordriver PROPERTY
             INTERFACE_motordriver_MAJOR_VERSION "${motordriver_VERSION_MAJOR}")

## Hide all symbols if not exported
generate_export_header(motordriver)
set_target_properties(${TARGET} PROPERTIES CXX_VISIBILITY_PRESET hidden)
set_target_properties(${TARGET} PROPERTIES VISIBILITY_INLINES_HIDDEN 1)



## adding the source files
target_sources(motordriver PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/motordriver.cpp)
## adding includes
target_include_directories(motordriver PUBLIC
                           "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include;${CMAKE_CURRENT_BINARY_DIR}>"
                           "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/include>")
## define public headers for install
set(motordriver_PUBLIC_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/include/motordriver.h"
                               "${CMAKE_CURRENT_BINARY_DIR}/motordriver_export.h")
set_target_properties(motordriver PROPERTIES PUBLIC_HEADER "${motordriver_PUBLIC_HEADERS}")

install(TARGETS motordriver
        EXPORT motordriverTargets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

## Install proper (cmake) package
install(EXPORT motordriverTargets
        FILE motordriverTargets.cmake
        NAMESPACE 1x::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/motordriver
)

configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/motordriver-config.cmake.in
        "${CMAKE_CURRENT_BINARY_DIR}/motordriverConfig.cmake"
        INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/motordriver
)


# generate the version file for the config file
write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/motordriverConfigVersion.cmake"
        VERSION "${motordriver_VERSION}"
        COMPATIBILITY AnyNewerVersion
)

install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/motordriverConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/motordriverConfigVersion.cmake"
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/motordriver
)