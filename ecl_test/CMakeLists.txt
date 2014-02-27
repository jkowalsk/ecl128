cmake_minimum_required (VERSION 2.8)
project (ecl_test)

# include custom cmake definition
include("../cmake/common.cmake")

set(EXE_NAME ecl_test)

#### compilation flags
set(CMAKE_CXX_FLAGS_DEBUG 
  "${CMAKE_CXX_FLAGS_DEBUG} -DFORCE_NO_ASM -DGTEST_HAS_PTHREAD=0 "
  )
  
set(CMAKE_CXX_FLAGS_RELEASE
  "${CMAKE_CXX_FLAGS_RELEASE} -DGTEST_HAS_PTHREAD=0"
  )
  
if(MSVC)
  add_definitions(-D_VARIADIC_MAX=10)
endif(MSVC)
 
#### source file definition
set( COMMON_TEST_FILES
  src/main.cpp
  src/Tools/clock.cpp
  src/Tools/rand.cpp
)

set( UTIL_TEST_FILES
    src/utils/buffer_test.cpp
  )

set( FP_TEST_FILES
  )

set( FIELD_TEST_FILES  
    src/field/Field_test.cpp
  )

set( CURVE_TEST_FILES  
    src/curve/curve_test.cpp 
    src/curve/bncurve_test.cpp 
    src/curve/bnpairing_test.cpp 
  )

set( DIGEST_TEST_FILES
    src/digest/sha256_test.cpp
  )

set( GTEST_SRC_FILES
    src/gtest-all.cpp
    )

set( SRC_FILES
   ${COMMON_TEST_FILES}
   ${UTIL_TEST_FILES}
   ${FIELD_TEST_FILES}
   ${CURVE_TEST_FILES}   
   ${DIGEST_TEST_FILES}   
   ${GTEST_SRC_FILES}
)

set( INCLUDE_FILES
  include/rand.h
  include/clock.h
  include/config.h
  include/gtest/gtest.h
  )

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  )

set(ALL_FILES
  ${SRC_FILES}
  ${INCLUDE_FILES}
  )
  
add_executable(${EXE_NAME}
  ${ALL_FILES}
)

if(MSVC)
#GroupFiles(${ALL_FILES})
    foreach(FILE ${ALL_FILES}) 
      get_filename_component(PARENT_DIR "${FILE}" PATH)
      # skip src or include and changes /'s to \\'s
      string(REGEX REPLACE "(\\./)?(src|include)/?" "" GROUP "${PARENT_DIR}")
      string(REPLACE "/" "\\" GROUP "${GROUP}")
      # group into "Source Files" and "Header Files"
      if ("${FILE}" MATCHES ".*\\.cpp")
        set(GROUP "Source Files\\${GROUP}")
      elseif("${FILE}" MATCHES ".*\\.h")
        set(GROUP "Header Files\\${GROUP}")
      endif()
      source_group("${GROUP}" FILES "${FILE}")
      endforeach()
endif(MSVC)

##### dependencies
set(ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/..")
set(ECL_DIR "${ROOT_DIR}/ecl")

add_subdirectory(${ECL_DIR} ${CMAKE_CURRENT_BINARY_DIR}/ecl)
include_directories(${ECL_DIR}/include)

target_link_libraries(${EXE_NAME}
  ecl 
)
