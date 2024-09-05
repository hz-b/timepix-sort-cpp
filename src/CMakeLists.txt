# Todo: correct place here or one further up?
set(timepix_sort_CMAKE_LIB_DIR "${CMAKE_INSTALL_LIBDIR}/cmake/gtpsa")

set(timepix_sort_cpp_HEADERS
  timepix_sort/timepix_sort.h
  timepix_sort/data_model.h
  timepix_sort/data_model_chunks.h
  timepix_sort/utils.h
  timepix_sort/read.h
  timepix_sort/process.h
  timepix_sort/events.h
  )

set(timepix_sort_cpp_detail_HEADERS
  timepix_sort/detail/process_chunks.h
  )

install(FILES ${timepix_sort_cpp_detail_HEADERS}
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/timepix_sort/detail/
  )

install(FILES ${timepix_sort_cpp_HEADERS}
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/timepix_sort/
  )


set(timepix_sort_cpp_FILES
  data_model.cpp
  read.cpp
  process.cpp
  )


add_library(timepix-sort-c++ SHARED
  ${timepix_sort_cpp_FILES}
  ${timepix_sort_cpp_HEADERS}
  ${timepix_sort_cpp_detail_HEADERS}
  )

add_library(timepix-sort::cpp ALIAS timepix-sort-c++)

target_link_libraries(timepix-sort-c++ ${OpenMP_C_LIBRARIES})

target_include_directories(timepix-sort-c++ PUBLIC
  #$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/>
  $<INSTALL_INTERFACE:include/>  # <prefix>/include/mylib
  ${ARMADILLO_INCLUDE_DIRS}
  )


install(TARGETS timepix-sort-c++
        EXPORT timepix-sort-cppTargets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
    EXPORT timepix-sort-cppTargets
    FILE timepix-sort-cppTargets.cmake
    DESTINATION ${timepix_sort_CMAKE_LIB_DIR}
    NAMESPACE timepix-sort::
    )
