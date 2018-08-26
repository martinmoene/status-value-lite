# Copyright 2016-2018 by Martin Moene
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required( VERSION 3.5 )

project( test LANGUAGES CXX )

# Test sources:

set( MAIN_BASE   status_value )
set( MAIN_BASE98 status_value_cpp98 )
set( HDRNAME     status_value.hpp )
set( SOURCES     ${MAIN_BASE}.t.cpp )
set( SOURCES98   ${MAIN_BASE98}.t.cpp )

set( HDRDIR  ${PROJECT_SOURCE_DIR}/../include/nonstd )
set( HDRPATH ${HDRDIR}/${HDRNAME} )

# Configure status_value for testing:

set( OPTIONS "" )
set( NSSV_CONFIG -Dlest_FEATURE_AUTO_REGISTER=1 )

# Preset available C++ language compiler flags:

set( HAS_STD_FLAGS  FALSE )
set( HAS_CPP98_FLAG FALSE )
set( HAS_CPP11_FLAG FALSE )
set( HAS_CPP14_FLAG FALSE )
set( HAS_CPP17_FLAG FALSE )
set( HAS_CPP20_FLAG FALSE )
set( HAS_CPPLATEST_FLAG FALSE )

if( MSVC )
    set( HAS_STD_FLAGS TRUE )

    set( OPTIONS     -W3 -EHsc )
    set( DEFINITIONS -D_SCL_SECURE_NO_WARNINGS -DNOMINMAX ${NSSV_CONFIG} )

    if( NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 19.00 )
        set( HAS_CPP14_FLAG TRUE )
        set( HAS_CPPLATEST_FLAG TRUE )
    endif()
    if( NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 19.11 )
        set( HAS_CPP17_FLAG TRUE )
    endif()

elseif( CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang" )

    set( HAS_STD_FLAGS  TRUE )
    set( HAS_CPP98_FLAG TRUE )

    set( OPTIONS     -Wall -Wno-missing-braces -fno-elide-constructors )
    set( DEFINITIONS ${NSSV_CONFIG} )

    # GNU: available -std flags depends on version
    if( CMAKE_CXX_COMPILER_ID MATCHES "GNU" )
        if( NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.8.0 )
            set( HAS_CPP11_FLAG TRUE )
        endif()
        if( NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.9.2 )
            set( HAS_CPP14_FLAG TRUE )
        endif()
        if( NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 7.1.0 )
            set( HAS_CPP17_FLAG TRUE )
        endif()
    endif()

    # Clang: available -std flags depends on version
    if( CMAKE_CXX_COMPILER_ID MATCHES "Clang" )
        if( NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 3.3.0 )
            set( HAS_CPP11_FLAG TRUE )
        endif()
        if( NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 3.4.0 )
            set( HAS_CPP14_FLAG TRUE )
        endif()
        if( NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 5.0.0 )
            set( HAS_CPP17_FLAG TRUE )
        endif()
    endif()

elseif( CMAKE_CXX_COMPILER_ID MATCHES "Intel" )
# as is
else()
# as is
endif()

# enable MS C++ Core Guidelines checker if MSVC:

function( enable_msvs_guideline_checker target )
    if( MSVC )
        set_target_properties( ${target} PROPERTIES
        VS_GLOBAL_EnableCppCoreCheck true
        VS_GLOBAL_CodeAnalysisRuleSet CppCoreCheckRules.ruleset
        VS_GLOBAL_RunCodeAnalysis true )
    endif()
endfunction()

# make target, compile for given standard if specified:

function( make_target target sources std )
    add_executable            ( ${target} ${sources} ${HDRPATH} )
    target_compile_options    ( ${target} PRIVATE ${OPTIONS} )
    target_compile_definitions( ${target} PRIVATE ${DEFINITIONS} )
    target_include_directories( ${target} PRIVATE ${HDRDIR} )
    if( std )
        if( MSVC )
            target_compile_options( ${target} PRIVATE -std:c++${std} )
        else()
            # Necessary for clang 3.x:
            target_compile_options( ${target} PRIVATE -std=c++${std} )
            # Ok for clang 4 and later:
            # set( CMAKE_CXX_STANDARD ${std} )
            # set( CMAKE_CXX_STANDARD_REQUIRED ON )
            # set( CMAKE_CXX_EXTENSIONS OFF )
        endif()
    endif()
endfunction()

# add generic executable, unless -std flags can be specified:

# C++11 version:

if( NOT HAS_STD_FLAGS )
#    make_target( ${MAIN_BASE}.t "${SOURCES}" "" )
else()
    if( HAS_CPP11_FLAG )
        make_target( ${MAIN_BASE}-cpp11.t "${SOURCES}" 11 )
    endif()

    if( HAS_CPP14_FLAG )
        make_target( ${MAIN_BASE}-cpp14.t "${SOURCES}" 14 )
    endif()

    if( HAS_CPP17_FLAG )
        make_target( ${MAIN_BASE}-cpp17.t "${SOURCES}" 17 )
        enable_msvs_guideline_checker( ${MAIN_BASE}-cpp17.t )
    endif()

    if( HAS_CPPLATEST_FLAG )
        make_target( ${MAIN_BASE}-cpplatest.t "${SOURCES}" latest )
    endif()
endif()

# C++98 version:

if( NOT HAS_STD_FLAGS )
    make_target( ${MAIN_BASE98}.t "${SOURCES98}" "" )
else()
    if( HAS_CPP98_FLAG )
        make_target( ${MAIN_BASE98}-cpp98.t "${SOURCES98}" 98 )
    endif()

    if( HAS_CPP11_FLAG )
        make_target( ${MAIN_BASE98}-cpp11.t "${SOURCES98}" 11 )
    endif()

    if( HAS_CPP14_FLAG )
        make_target( ${MAIN_BASE98}-cpp14.t "${SOURCES98}" 14 )
    endif()

    if( HAS_CPP17_FLAG )
        make_target( ${MAIN_BASE98}-cpp17.t "${SOURCES98}" 17 )
        enable_msvs_guideline_checker( ${MAIN_BASE98}-cpp17.t )
    endif()

    if( HAS_CPPLATEST_FLAG )
        make_target( ${MAIN_BASE98}-cpplatest.t "${SOURCES98}" latest )
    endif()
endif()

# configure unit tests via CTest:

enable_testing()

# C++11 version:

if( HAS_STD_FLAGS )
    if( HAS_CPP11_FLAG )
        add_test( NAME test-cpp11     COMMAND ${MAIN_BASE}-cpp11.t )
    endif()
    if( HAS_CPP14_FLAG )
        add_test( NAME test-cpp14     COMMAND ${MAIN_BASE}-cpp14.t )
    endif()
    if( HAS_CPP17_FLAG )
        add_test( NAME test-cpp17     COMMAND ${MAIN_BASE}-cpp17.t )
    endif()
    if( HAS_CPPLATEST_FLAG )
        add_test( NAME test-cpplatest COMMAND ${MAIN_BASE}-cpplatest.t )
    endif()
else()
#    add_test(     NAME test           COMMAND ${MAIN_BASE}.t --pass )
#    add_test(     NAME list_version   COMMAND ${MAIN_BASE}.t --version )
#    add_test(     NAME list_tags      COMMAND ${MAIN_BASE}.t --list-tags )
#    add_test(     NAME list_tests     COMMAND ${MAIN_BASE}.t --list-tests )
endif()

# C++98 version:

if( HAS_STD_FLAGS )
    if( HAS_CPP98_FLAG )
        add_test( NAME test98-cpp98     COMMAND ${MAIN_BASE98}-cpp98.t )
    endif()
    if( HAS_CPP11_FLAG )
        add_test( NAME test98-cpp11     COMMAND ${MAIN_BASE98}-cpp11.t )
    endif()
    if( HAS_CPP14_FLAG )
        add_test( NAME test98-cpp14     COMMAND ${MAIN_BASE98}-cpp14.t )
    endif()
    if( HAS_CPP17_FLAG )
        add_test( NAME test98-cpp17     COMMAND ${MAIN_BASE98}-cpp17.t )
    endif()
    if( HAS_CPPLATEST_FLAG )
        add_test( NAME test98-cpplatest COMMAND ${MAIN_BASE98}-cpplatest.t )
    endif()
else()
    add_test(     NAME test98-cpp98     COMMAND ${MAIN_BASE98}.t --pass )
    add_test(     NAME list98_version   COMMAND ${MAIN_BASE98}.t --version )
    add_test(     NAME list98_tags      COMMAND ${MAIN_BASE98}.t --list-tags )
    add_test(     NAME list98_tests     COMMAND ${MAIN_BASE98}.t --list-tests )
endif()

# end of file
