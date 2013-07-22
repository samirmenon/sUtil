cmake_minimum_required(VERSION 2.6)

#Name the project
project(sutil)

#Set Include directories
SET(SUTIL_TEST_DIR ./)
SET(SUTIL_INC_DIR ../../src/)

INCLUDE_DIRECTORIES(${SUTIL_INC_DIR})

#Set all the sources required for the tests
SET(ALLSRC ${SUTIL_TEST_DIR}/test_main.cpp
           ${SUTIL_TEST_DIR}/test-callbacks.cpp
           ${SUTIL_TEST_DIR}/test-dyntypes.cpp
           ${SUTIL_TEST_DIR}/test-mappedlist.cpp
           ${SUTIL_TEST_DIR}/test-mapped-tree.cpp
           ${SUTIL_TEST_DIR}/test-memcopier.cpp
           ${SUTIL_TEST_DIR}/test-objhist.cpp
           ${SUTIL_TEST_DIR}/test-printables.cpp
           ${SUTIL_TEST_DIR}/test-shmem.cpp
           ${SUTIL_TEST_DIR}/test-singleton.cpp
   )

#Set the build mode to debug by default
#SET(CMAKE_BUILD_TYPE Debug)

IF(CMAKE_BUILD_TYPE MATCHES Debug)
  #Add debug definitions
  ADD_DEFINITIONS(-DASSERT=assert -DDEBUG=1 -DW_TESTING=1)
  SET(CMAKE_CXX_FLAGS_DEBUG "-Wall -Woverloaded-virtual -ggdb -O0 -pg -std=c++0x -fopenmp")
ENDIF(CMAKE_BUILD_TYPE MATCHES Debug)

IF(CMAKE_BUILD_TYPE MATCHES Release)
  #Add release definitions
  ADD_DEFINITIONS( -DEIGEN_NO_DEBUG )
  SET(CMAKE_CXX_FLAGS_RELEASE "-Wall -Woverloaded-virtual -O3 -std=c++0x -fopenmp")
ENDIF(CMAKE_BUILD_TYPE MATCHES Release)

#Make sure the generated makefile is not shortened
SET(CMAKE_VERBOSE_MAKEFILE ON)

#Define the library to be built
add_executable(sutil_test ${ALLSRC})
