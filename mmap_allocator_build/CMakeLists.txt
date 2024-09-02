# use cmake to build the mmap allocator

set(mmap_allocator_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../external_repositories/mmap_allocator/)

set(mmap_allocator_HEADERS
  ${mmap_allocator_SRC_DIR}/mmap_access_mode.h
  ${mmap_allocator_SRC_DIR}/mmap_file_pool.h
  ${mmap_allocator_SRC_DIR}/mmap_allocator.h
  ${mmap_allocator_SRC_DIR}/mmappable_vector.h
  ${mmap_allocator_SRC_DIR}/mmap_exception.h
  )

set(mmap_allocator_FILES
  ${mmap_allocator_SRC_DIR}/mmap_file_pool.cpp
  )

add_library(mmap_allocator SHARED
  ${mmap_allocator_FILES}
  ${mmap_allocator_HEADERS}
)
target_link_libraries(mmap_allocator)

target_include_directories(mmap_allocator
                           PUBLIC
                           "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
                           "$<BUILD_INTERFACE:${mmap_allocator_SRC_DIR}>"
                           "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

install(TARGETS mmap_allocator
        EXPORT mmap_allocatorTargets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
	)

set(mmap_allocator_CMAKE_LIB_DIR "${CMAKE_INSTALL_LIBDIR}/cmake/mmap_allocator")
install(
    EXPORT mmap_allocatorTargets
    FILE mmap_allocatorTargets.cmake
    DESTINATION ${mmap_allocator_CMAKE_LIB_DIR}
    NAMESPACE mmap_allocator::
)

install(FILES ${mmap_allocator_HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
