# Copyright 2016-2018 by Martin Moene
#
# https://github.com/martinmoene/status-value
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required( VERSION 3.5 )

# status-value project and version, updated by script/update-version.py:

project(
    status_value
    VERSION 1.0.0
    DESCRIPTION "A class for status and optional value for C++11 and later, C++98 variant provided in a single-file header-only library"
#   HOMEPAGE_URL "https://github.com/martinmoene/status-value"
    LANGUAGES CXX )

set( ${PROJECT_NAME}_version "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}" )

# Toplevel or subproject:

if ( CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME )
    set( status_value_IS_TOPLEVEL_PROJECT TRUE )
else()
    set( status_value_IS_TOPLEVEL_PROJECT FALSE )
endif()

# If toplevel project, enable building and performing of tests, disable building of examples:

option( NSSV_OPT_BUILD_TESTS    "Build and perform status_value tests" ${status_value_IS_TOPLEVEL_PROJECT} )
option( NSSV_OPT_BUILD_EXAMPLES "Build status_value examples" OFF )

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
    DIRECTORY   ${CMAKE_SOURCE_DIR}/include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} )

# If requested, build and perform tests, build examples:

enable_testing()

if ( NSSV_OPT_BUILD_TESTS )
    add_subdirectory( test )
endif()

if ( NSSV_OPT_BUILD_EXAMPLES )
    add_subdirectory( example )
endif()

# end of file
