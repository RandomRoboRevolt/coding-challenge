add_executable(master)
target_sources(master PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)
target_link_libraries(master motordriver)
install(TARGETS master
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
