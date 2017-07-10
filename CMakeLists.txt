###################
# cmake file to build the OfflineAnalysis for gent RPC
###################
CMAKE_MINIMUM_REQUIRED(VERSION 2.8 FATAL_ERROR)

# -------------------------------------------------
# Project Name/Version
PROJECT(GentRPCOfflineAnalysis)
SET(${PROJECT_NAME}_VERSION_MAJOR 2)
SET(${PROJECT_NAME}_VERSION_MINOR 0)

# -------------------------------------------------
# Require C++11
INCLUDE( CheckCXXCompilerFlag )
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)

IF( COMPILER_SUPPORTS_CXX11 )
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
ELSE()
	MESSAGE( SEND_ERROR "${PROJECT_NAME} requires C++11 support. Please upgrade your compiler !" )
ENDIF()
# -------------------------------------------------

# -------------------------------------------------
# Require proper c++
SET( ${CMAKE_CXX_FLAGS} "${CMAKE_CXX_FLAGS} -pedantic -O2 -Wall -Werror" )

# -------------------------------------------------
# CMake magic for shared lib linking after install (https://cmake.org/Wiki/CMake_RPATH_handling)
# add install path to the rpath list (apple)
IF( APPLE )
  SET( CMAKE_MACOSX_RPATH TRUE )
ENDIF()

# use, i.e. don't skip the full RPATH for the build tree
SET(CMAKE_SKIP_BUILD_RPATH  FALSE)

# when building, don't use the install RPATH already
# (but later on when installing)
SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE) 
SET(CMAKE_INSTALL_RPATH "${PROJECT_SOURCE_DIR}/lib")

# add the automatically determined parts of the RPATH
# which point to directories outside the build tree to the install RPATH
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
# Enf of CMake magic
# -------------------------------------------------


# -------------------------------------------------
# cmake macros to look for dependencies
LIST( APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake )

# -------------------------------------------------
# ----- Find ROOT package -----
LIST( APPEND ROOT_COMPONENT_LIST Core Hist HistPainter Tree )
FIND_PACKAGE( ROOT COMPONENTS ${ROOT_COMPONENT_LIST} REQUIRED )

FOREACH( pkg ROOT )
  INCLUDE_DIRECTORIES( ${${pkg}_INCLUDE_DIRS} )
  LINK_LIBRARIES( ${${pkg}_LIBRARIES} )
  ADD_DEFINITIONS ( ${${pkg}_DEFINITIONS} )

  LIST( APPEND ${PROJECT_NAME}_DEPENDS_INCLUDE_DIRS ${${pkg}_INCLUDE_DIRS} )
  LIST( APPEND ${PROJECT_NAME}_DEPENDS_LIBRARY_DIRS ${${pkg}_LIBRARY_DIRS} )
  LIST( APPEND ${PROJECT_NAME}_DEPENDS_LIBRARIES ${${pkg}_LIBRARIES} )
ENDFOREACH()

# -------------------------------------------------
# include directories
INCLUDE_DIRECTORIES( ${PROJECT_SOURCE_DIR}/include )
# Add source directory
AUX_SOURCE_DIRECTORY( src SRC_FILES )
ADD_LIBRARY( ${PROJECT_NAME} SHARED ${SRC_FILES} )
INSTALL( 
    TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION ${PROJECT_SOURCE_DIR}/lib
)

# -------------------------------------------------
# Build/Install executable
ADD_EXECUTABLE( OfflineAnalysis src/main.cc )
TARGET_LINK_LIBRARIES( OfflineAnalysis ${PROJECT_NAME} )

INSTALL (
    TARGETS OfflineAnalysis
    RUNTIME DESTINATION ${PROJECT_SOURCE_DIR}/bin
)

