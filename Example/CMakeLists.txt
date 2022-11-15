# Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0
cmake_minimum_required( VERSION 3.20 FATAL_ERROR )

project( NodeViewExample )

find_package( thinkboxcmlibrary REQUIRED )
include( ThinkboxCMLibrary )

set( CMAKE_AUTOMOC ON )
set( CMAKE_AUTORCC ON )
set( CMAKE_AUTOUIC ON )

set( H_FILES mainwindow.h )
set( CXX_FILES main.cpp mainwindow.cpp )

add_executable( nodeviewexample )
target_sources( nodeviewexample PRIVATE ${H_FILES} ${CXX_FILES} ${MOC_SOURCE_FILES} ${UI_HEADER_FILES} mainwindow.ui )

find_package( Qt5 COMPONENTS Core Gui Widgets REQUIRED )

target_include_directories( nodeviewexample PUBLIC ${Qt5_INCLUDE_DIRS} )

target_link_libraries( nodeviewexample INTERFACE Qt5::Core )
target_link_libraries( nodeviewexample INTERFACE Qt5::Gui )
target_link_libraries( nodeviewexample INTERFACE Qt5::Widgets )

target_link_libraries( nodeviewexample PRIVATE nodeview )

set_target_properties( nodeviewexample PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${NodeView_BINARY_DIR} )

install( TARGETS nodeviewexample
            RUNTIME DESTINATION bin
            LIBRARY DESTINATION lib
            ARCHIVE DESTINATION lib )
