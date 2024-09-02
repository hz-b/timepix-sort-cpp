set(timepix_sort_test_cpp_FILES
  read_raw.cpp
  )

add_library(timepix-sort-test-c++ SHARED
  ${timepix_sort_test_cpp_FILES}
)

target_include_directories(timepix-sort-test-c++ PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR/mmap_allocator_SRC_DIR}>
  $<INSTALL_INTERFACE:include/>
  )

target_link_libraries(timepix-sort-test-c++ timepix-sort-c++ mmap_allocator)


# add_executable(test_consistency test_consistency.cc)

# add_tests()


install(TARGETS timepix-sort-test-c++
        EXPORT timepix-sort-test-c++Targets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
	)

set(timepix_sort_CMAKE_LIB_DIR "${CMAKE_INSTALL_LIBDIR}/cmake/timepix_sort")
install(
    EXPORT timepix-sort-test-c++Targets
    FILE timepix-sort-test-c++Targets.cmake
    DESTINATION ${timepix_sort_CMAKE_LIB_DIR}
    NAMESPACE timepix_sort::
)

add_executable(main main.cpp)
target_link_libraries(main timepix-sort-test-c++ timepix-sort-c++ mmap_allocator)

add_executable(sort_test sort_test.cpp)
target_link_libraries(sort_test timepix-sort-c++)
