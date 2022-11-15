# Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0
cmake_minimum_required( VERSION 3.20 FATAL_ERROR )

project( NodeView )

find_package( thinkboxcmlibrary REQUIRED )
include( ThinkboxCMLibrary )

set( SUPPORTED_MAYA_VERSIONS 2022 2023 )
 
option( MAYA_VERSION "The version of QtMaya to build the library against." 2022 )
option( BUILD_EXAMPLES "Build the example NodeView project" OFF )

if( NOT MAYA_VERSION IN_LIST SUPPORTED_MAYA_VERSIONS )
  message( FATAL_ERROR "ERROR: Cannot build for unsupported Maya version ${MAYA_VERSION}" )
endif()

set( CMAKE_AUTOMOC ON )
set( CMAKE_AUTORCC ON )
set( CMAKE_AUTOUIC ON )

add_library( nodeview STATIC )

target_compile_definitions( nodeview PUBLIC FRANTIC_BUILD_FOR_MAYA )

file( GLOB_RECURSE H_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
        "NodeView/*.h"
        "NodeView/*.hpp"
)

file( GLOB_RECURSE CXX_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
        "src/*.cpp"
)

target_sources( nodeview PRIVATE ${H_FILES} ${CXX_FILES} "resources/images.qrc" )


target_include_directories( nodeview PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
                            $<INSTALL_INTERFACE:include> )

find_package( Qt5 COMPONENTS Core Gui Widgets REQUIRED )

target_include_directories( nodeview PUBLIC ${Qt5_INCLUDE_DIRS} )

target_link_libraries( nodeview INTERFACE Qt5::Core )
target_link_libraries( nodeview INTERFACE Qt5::Gui )
target_link_libraries( nodeview INTERFACE Qt5::Widgets )

frantic_common_platform_setup( nodeview )
frantic_default_source_groups( nodeview HEADERDIR NodeView SOURCEDIR src )

# Disable optimization for the RelWithDebInfo configuration on Windows.
# This allows breakpoints to be hit reliably when debugging in Visual Studio.
if( WIN32 )
	target_compile_options( nodeview PRIVATE "$<$<CONFIG:Release>:/O2>$<$<CONFIG:RelWithDebInfo>:/Od>" )
endif()
  
install( DIRECTORY NodeView
          DESTINATION include
          FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp"
)
install( TARGETS nodeview
          RUNTIME DESTINATION bin
          LIBRARY DESTINATION lib
          ARCHIVE DESTINATION lib
)

if( BUILD_EXAMPLES )
    add_subdirectory( Example )
endif()
