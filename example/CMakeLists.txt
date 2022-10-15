# Copyright 2016-2018 by Martin Moene
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

project( example LANGUAGES CXX )

# unit_name provided by toplevel CMakeLists.txt
set( PACKAGE   "${unit_name}-lite" )

message( STATUS "Subproject '${PROJECT_NAME}'")

set( SOURCES_CPP98
    01-basic_cpp98.cpp
)

set( SOURCES_CPP11
    02-required.cpp
    03-error_condition.cpp
)

set( SOURCES_CPP14
    01-basic.cpp
)

# note: here variable must be quoted to create semicolon separated list:

string( REPLACE ".cpp" "" BASENAMES_CPP98 "${SOURCES_CPP98}" )
string( REPLACE ".cpp" "" BASENAMES_CPP11 "${SOURCES_CPP11}" )
string( REPLACE ".cpp" "" BASENAMES_CPP14 "${SOURCES_CPP14}" )

set( TARGETS_CPP98 ${BASENAMES_CPP98} )
set( TARGETS_CPP11 ${BASENAMES_CPP11} )
set( TARGETS_CPP14 ${BASENAMES_CPP14} )
set( TARGETS_CPP17 ${BASENAMES_CPP17} )
set( TARGETS_CPP12 ${BASENAMES_CPP20} )
set( TARGETS_ALL   ${TARGETS_CPP98} ${TARGETS_CPP11} ${TARGETS_CPP14} ${TARGETS_CPP17} ${TARGETS_CPP20} )

# Configure status_value for testing:

set( OPTIONS "" )
set( NSSTSV_CONFIG -Dlest_FEATURE_AUTO_REGISTER=1 )

# Preset available C++ language compiler flags:

set( HAS_CPP98_FLAG FALSE )
set( HAS_CPP11_FLAG FALSE )
set( HAS_CPP14_FLAG FALSE )
set( HAS_CPP17_FLAG FALSE )
set( HAS_CPP20_FLAG FALSE )
set( HAS_CPPLATEST_FLAG FALSE )

if( MSVC )
    message( STATUS "Matched: MSVC")

    set( HAS_STD_FLAGS TRUE )

    set( OPTIONS     -W3 -EHsc -wd4814 -Zc:implicitNoexcept- )
    set( DEFINITIONS -D_SCL_SECURE_NO_WARNINGS ${NSSTSV_CONFIG} )

    if( NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 19.00 )
        set( HAS_CPP14_FLAG TRUE )
        set( HAS_CPPLATEST_FLAG TRUE )
    endif()
    if( NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 19.11 )
        set( HAS_CPP17_FLAG TRUE )
    endif()

elseif( CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang|AppleClang" )
    message( STATUS "CompilerId: '${CMAKE_CXX_COMPILER_ID}'")

    set( HAS_STD_FLAGS  TRUE )
    set( HAS_CPP98_FLAG TRUE )

    set( OPTIONS     -Wall -Wno-missing-braces -fno-elide-constructors )
    set( DEFINITIONS ${NSSTSV_CONFIG} )

    # GNU: available -std flags depends on version
    if( CMAKE_CXX_COMPILER_ID MATCHES "GNU" )
        message( STATUS "Matched: GNU")

        if( NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.8.0 )
            set( HAS_CPP11_FLAG TRUE )
        endif()
        if( NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.9.2 )
            set( HAS_CPP14_FLAG TRUE )
        endif()
        if( NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 7.1.0 )
            set( HAS_CPP17_FLAG TRUE )
        endif()

    # AppleClang: available -std flags depends on version
    elseif( CMAKE_CXX_COMPILER_ID MATCHES "AppleClang" )
        message( STATUS "Matched: AppleClang")

        if( NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 5.0.0 )
            set( HAS_CPP11_FLAG TRUE )
        endif()
        if( NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 5.1.0 )
            set( HAS_CPP14_FLAG TRUE )
        endif()
        if( NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.2.0 )
            set( HAS_CPP17_FLAG TRUE )
        endif()

    # Clang: available -std flags depends on version
    elseif( CMAKE_CXX_COMPILER_ID MATCHES "Clang" )
        message( STATUS "Matched: Clang")

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
    message( STATUS "Matched: Intel")
else()
    # as is
    message( STATUS "Matched: nothing")
endif()

# make target, compile for given standard if specified:

function( make_target target sources std )
    add_executable            ( ${target} ${sources} )
    target_link_libraries     ( ${target} PRIVATE ${PACKAGE} )
    target_compile_options    ( ${target} PRIVATE ${OPTIONS} )
    target_compile_definitions( ${target} PRIVATE ${DEFINITIONS} )

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

# set compiler options:

if( HAS_CPPLATEST_FLAG )
    foreach( name ${TARGETS_ALL} )
        make_target( ${name}-c++latest.e ${name} c++latest )
    endforeach()

elseif( HAS_CPP20_FLAG )
    foreach( name ${TARGETS_ALL} )
        make_target( ${name}-c++20.e ${name} 20 )
    endforeach()

elseif( HAS_CPP17_FLAG )
    foreach( name ${TARGETS_ALL} )
        make_target( ${name}-c++17.e ${name} 17 )
    endforeach()

elseif( HAS_CPP14_FLAG )
    foreach( name ${TARGETS_CPP98} ${TARGETS_CPP11} ${TARGETS_CPP14} )
        make_target( ${name}-c++14.e ${name} 14 )
    endforeach()

elseif( HAS_CPP11_FLAG )
    foreach( name ${TARGETS_CPP98} ${TARGETS_CPP11} )
        make_target( ${name}-c++11.e ${name} 11 )
    endforeach()

elseif( HAS_CPP98_FLAG )
    foreach( name ${TARGETS_CPP98} )
        make_target( ${name}-c++98.e ${name} 98 )
    endforeach()

elseif( MSVC )
    foreach( name ${TARGETS_CPP98} )
        make_target( ${name}-c++98.e ${name} "" )
    endforeach()
endif()

# configure unit tests via CTest:

#enable_testing()
#
#foreach( name ${TARGETS_ALL} )
#    add_test    ( NAME ${name} COMMAND ${name} )
#    set_property( TEST ${name} PROPERTY LABELS example )
#endforeach()

# end of file
