if ( NOT CMAKE_BUILD_TYPE )
  set( CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type: release, debug, relwithdebinfo" FORCE )
endif()

cmake_minimum_required( VERSION 3.2 )
project( fecpp )

option( HAS_SSE2 "Compile for SSE2" ON )
option( HAS_SSSE3 "Compile for SSSE3" ON )

set( FECPP_SOURCES fecpp.cpp cpuid.cpp )
if ( HAS_SSE2 )
  add_definitions( -msse2 )
  list( APPEND FECPP_SOURCES fecpp_sse2.cpp )
endif()
if ( HAS_SSSE3 )
  add_definitions( -mssse3 )
  list( APPEND FECPP_SOURCES fecpp_ssse3.cpp )
endif()
add_definitions( -Wall -Wextra -std=c++11 )

add_library( fecpp-object OBJECT ${FECPP_SOURCES} )
set_property( TARGET fecpp-object PROPERTY POSITION_INDEPENDENT_CODE ON )
add_library( fecpp-static STATIC $<TARGET_OBJECTS:fecpp-object> )
add_library( fecpp        SHARED $<TARGET_OBJECTS:fecpp-object> )
set_target_properties( fecpp PROPERTIES PUBLIC_HEADER fecpp.h )

include( FindPkgConfig )
pkg_check_modules( PYTHON2 python2 )
if ( PYTHON2_FOUND )
  find_package( Boost COMPONENTS python )
  if ( Boost_FOUND )
    add_library( pyfecpp SHARED fecpp_python.cpp )
    target_include_directories( pyfecpp PUBLIC ${PYTHON2_INCLUDE_DIRS} )
    target_link_libraries( pyfecpp fecpp-static ${Boost_LIBRARIES} ${PYTHON2_LIBRARIES} )
    set_target_properties( pyfecpp PROPERTIES PREFIX "")
  else()
    message( WARN "boost_python library was not found. fecpp python library will not be built" )
  endif()
else()
  message( WARN "python2 configuration for pkg_config was not found. fecpp python library will not be built" )
endif()

include_directories( ${CMAKE_SOURCE_DIR} )
add_executable( gen_test_vec test/gen_test_vec.cpp )
target_link_libraries( gen_test_vec fecpp-static )

add_executable( test_recovery test/test_recovery.cpp )
target_link_libraries( test_recovery fecpp-static )

add_executable( benchmark test/benchmark.cpp )
target_link_libraries( benchmark fecpp-static )

add_executable( zfec test/zfec.cpp )
target_link_libraries( zfec fecpp-static )

install( TARGETS fecpp fecpp-static gen_test_vec test_recovery benchmark zfec pyfecpp
         ARCHIVE DESTINATION lib LIBRARY DESTINATION lib RUNTIME DESTINATION bin
         PUBLIC_HEADER DESTINATION include )
install( FILES fec_hash.py test_decode.py
         PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ
         DESTINATION bin )
