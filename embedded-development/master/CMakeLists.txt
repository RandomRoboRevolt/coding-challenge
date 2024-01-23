add_executable(master)
target_sources(master PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)
target_link_libraries(master motordriver sml::sml)
target_include_directories(master PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
target_include_directories(master PUBLIC })
install(TARGETS master
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
