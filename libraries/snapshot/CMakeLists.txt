file(GLOB_RECURSE HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp")

set( SOLUTION_FILES
    ${HEADERS})

add_custom_target( snapshot SOURCES ${SOLUTION_FILES})
