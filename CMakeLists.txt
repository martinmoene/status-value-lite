# Copyright 2016-2018 by Martin Moene
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required( VERSION 3.5 )

# status-value version, updated by script/update-version.py:

set( status_value_version "1.0.0" )

# Enable building and performing of tests and building of examples:

option( NSSV_OPT_BUILD_TESTS    "Build and perform status_value tests" OFF )
option( NSSV_OPT_BUILD_EXAMPLES "Build status_value examples" OFF )

project( status_value LANGUAGES CXX )

include( GNUInstallDirs )

set( package_name "status-value" )
set( include_source_dir "${PROJECT_SOURCE_DIR}/include" )

# Interface library:

add_library(
    ${package_name} INTERFACE )

target_include_directories(
    ${package_name} INTERFACE "$<BUILD_INTERFACE:${include_source_dir}>" )

# Installation:

install(
    DIRECTORY ${CMAKE_SOURCE_DIR}/include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} )

# If requested, build and perform tests, build examples:

enable_testing()

if ( NSSV_OPT_BUILD_TESTS )
    add_subdirectory( test )
endif()

if ( NSSV_OPT_BUILD_EXAMPLES )
    add_subdirectory( example )
endif()

# end of file
