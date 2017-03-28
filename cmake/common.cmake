cmake_minimum_required (VERSION 2.8)


# g++ variants
if(CMAKE_COMPILER_IS_GNUCXX)
  if (NOT CMAKE_BUILD_TYPE)
    message(STATUS "No build type selected, default to Release")
    set(CMAKE_BUILD_TYPE "Release")
  endif()

  message(STATUS "Using g++ as compiler")
  
  # common compilation flags
  set(COMMON_CXX_FLAGS "-Wall -fmessage-length=0 -fno-exceptions")
  
  # debug specific flags
  set(CMAKE_CXX_FLAGS_DEBUG
  "${CMAKE_CXX_FLAGS_DEBUG} ${COMMON_CXX_FLAGS} -ggdb"
  )
  
  # Release specific flags  
  set(CMAKE_CXX_FLAGS_RELEASE 
  "${CMAKE_CXX_FLAGS_RELEASE} ${COMMON_CXX_FLAGS}" 
  )
  # Some performance flags
  set(CMAKE_CXX_FLAGS_RELEASE 
  "${CMAKE_CXX_FLAGS_RELEASE} -O3  -fomit-frame-pointer -finline-functions" 
  )
  # Some performance flags (loop unrolling)
  set(CMAKE_CXX_FLAGS_RELEASE 
  "${CMAKE_CXX_FLAGS_RELEASE} -funroll-loops -funsafe-loop-optimizations -floop-block"
  )
  # Some performance flags (link time optimization)
  set(CMAKE_CXX_FLAGS_RELEASE 
  "${CMAKE_CXX_FLAGS_RELEASE} -flto"
  )
  
  set(CMAKE_CXX_FLAGS_RELEASE 
  "${CMAKE_CXX_FLAGS_RELEASE} -march=native"
  )
  
  SET(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS} -flto")
  SET(CMAKE_AR  "gcc-ar")
  SET(CMAKE_CXX_ARCHIVE_CREATE "<CMAKE_AR> qcs <TARGET> <LINK_FLAGS> <OBJECTS>")
  SET(CMAKE_CXX_ARCHIVE_FINISH   true)
  
endif(CMAKE_COMPILER_IS_GNUCXX)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)


if(MSVC)
  message(STATUS "Using Visual Studio as compiler")
  
  set_property(GLOBAL PROPERTY USE_FOLDERS ON) 
    
endif(MSVC)
