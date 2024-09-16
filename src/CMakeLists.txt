# Todo: correct place here or one further up?
set(timepix_sort_CMAKE_LIB_DIR "${CMAKE_INSTALL_LIBDIR}/cmake/timepix_sort")

set(timepix_sort_cpp_HEADERS
  timepix_sort/timepix_sort.h
  timepix_sort/data_model.h
  timepix_sort/data_model_chunks.h
  timepix_sort/event_statistics.h
  timepix_sort/utils.h
  timepix_sort/read.h
  timepix_sort/process.h
  timepix_sort/events.h
  )

set(timepix_sort_cpp_detail_HEADERS
  timepix_sort/detail/process_chunks.h
  )

set(timepix_sort_cpp_hardware_HEADERS
  timepix_sort/hardware/tpx3.h
  )

install(FILES ${timepix_sort_cpp_detail_HEADERS}
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/timepix_sort/detail/
  )

install(FILES ${timepix_sort_cpp_hardware_HEADERS}
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/timepix_sort/hardware/
  )

install(FILES ${timepix_sort_cpp_HEADERS}
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/timepix_sort/
  )


set(timepix_sort_cpp_FILES
  data_model.cpp
  event_statistics.cpp
  read.cpp
  process.cpp
  )


add_library(timepix-sort-c++ SHARED
  ${timepix_sort_cpp_FILES}
  ${timepix_sort_cpp_HEADERS}
  ${timepix_sort_cpp_detail_HEADERS}
  )

add_library(timepix_sort::cpp ALIAS timepix-sort-c++)

target_link_libraries(timepix-sort-c++ ${OpenMP_C_LIBRARIES})

target_include_directories(timepix-sort-c++ PUBLIC
  #$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/>
  $<INSTALL_INTERFACE:include/>  # <prefix>/include/mylib
  ${ARMADILLO_INCLUDE_DIRS}
  )


install(TARGETS timepix-sort-c++
        EXPORT timepix_sort_cppTargets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
    EXPORT timepix_sort_cppTargets
    FILE timepix_sort_cppTargets.cmake
    DESTINATION ${timepix_sort_CMAKE_LIB_DIR}
    NAMESPACE timepix-sort::
    )


set(timepix_sort_cpp_CMAKE_LIB_DIR "${CMAKE_INSTALL_LIBDIR}/cmake/timepix_sort")

configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/timepix_sort_cppConfig.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/timepix_sort_cppConfig.cmake"
    INSTALL_DESTINATION ${timepix_sort_cpp_CMAKE_LIB_DIR}
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/timepix_sort_cppConfigVersion.cmake
    VERSION "${TIMEPIX_SORT_CPP_VERSION}"
    COMPATIBILITY SameMajorVersion
    # check the destination file
    # INSTALL_DESTINATION ${timepix_sort_cpp_CMAKE_LIB_DIR}
)
  


install(FILES
         "${CMAKE_CURRENT_BINARY_DIR}/timepix_sort_cppConfig.cmake"
         "${CMAKE_CURRENT_BINARY_DIR}/timepix_sort_cppConfigVersion.cmake"
	 DESTINATION ${timepix_sort_cpp_CMAKE_LIB_DIR}
)
  
