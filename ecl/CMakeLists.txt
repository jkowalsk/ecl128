cmake_minimum_required (VERSION 2.8)

# include custom cmake definition
include("../cmake/common.cmake")

project (ecl)
set(LIBRARY_NAME ecl)
 
set(CMAKE_CXX_FLAGS_DEBUG 
  "${CMAKE_CXX_FLAGS_DEBUG} -DFORCE_NO_ASM"
  )
  
  
set( BN_INCLUDE
    include/ecl/bigint/FixedSizedInt.h
    )
     
set(BN_FILES
    src/bigint/FixedSizedInt_4.cpp
    src/bigint/FixedSizedInt_8.cpp
    src/bigint/FixedSizedInt_16.cpp
)

set( FP_INCLUDE
    include/ecl/field/GFp.h
    include/ecl/field/Fp2.h
    include/ecl/field/Fp6.h
    include/ecl/field/Fp12.h
    )

set( FP_FILES
    src/field/gfp_base.cpp
    src/field/gfp_addsub.cpp
    src/field/gfp_cmp.cpp
    src/field/gfp_exp.cpp
    src/field/gfp_inv.cpp
    src/field/gfp_mul.cpp
    src/field/gfp_red.cpp
    src/field/gfp_sqrt.cpp    
    src/field/fp2_base.cpp
    src/field/fp2_mul.cpp
    src/field/fp6_base.cpp
    src/field/fp6_mul.cpp
    src/field/fp12_base.cpp
    src/field/fp12_mul.cpp
)

set( CURVE_INCLUDE
    include/ecl/curve/curve.h
    include/ecl/curve/bncurve.h
    include/ecl/curve/bnpairing.h
    )

set( CURVE_FILES
    src/curve/curve.cpp
    src/curve/bncurve.cpp
    src/curve/bnpairing.cpp
)

set( SRC_FILES
    ${BN_FILES}
    ${FP_FILES}
    ${CURVE_FILES}
)

set( INCLUDE_FILES
    include/ecl/config.h
    include/ecl/errcode.h
    ${BN_INCLUDE}
    ${FP_INCLUDE}
    ${CURVE_INCLUDE}
)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  )

set(ALL_FILES
  ${SRC_FILES}
  ${INCLUDE_FILES}
  ${PRIVATE_INCLUDE_FILES}
)

add_library(${LIBRARY_NAME} SHARED
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
