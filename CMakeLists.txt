# Copyright (C) 2016 Martin Moene.
#
# Distributed under the Boost Software License, Version 1.0. 
# (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required( VERSION 2.8 )

# At default, enable building and performing of tests and building of examples:

option( NSSV_OPT_BUILD_TESTS    "Build and perform status_value tests" ON )
option( NSSV_OPT_BUILD_EXAMPLES "Build status_value examples" ON )

project( status_value )

enable_testing()

# If requested, build and perform tests, build examples:

if ( NSSV_OPT_BUILD_TESTS )
    add_subdirectory( test )
endif()

if ( NSSV_OPT_BUILD_EXAMPLES )
    add_subdirectory( example )
endif()

# end of file
