// Copyright (C) 2016 Martin Moene.
//
// This version targets C++11 and later.
//
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// status_value is based on:
//   A Class for Status and Optional Value, P0262r0
//   by Lawrence Crowl and Chris Mysen

#include "status_value.hpp"

#include "lest.hpp"

#define CASE( name ) lest_CASE( specification, name )

static lest::tests specification;

using namespace nonstd;

struct not_default_constructible
{
    not_default_constructible() = delete;
};

struct move_constructible
{
    int x;
    move_constructible( int x ) : x(x) {}
    move_constructible( move_constructible && other ) : x( std::move( other.x ) ) {}
    move_constructible( move_constructible const & other ) = delete;
};

struct copy_constructible
{
    int x;
    copy_constructible( int x ) : x(x) {}
    copy_constructible( copy_constructible && other ) = delete;
    copy_constructible( copy_constructible const & other )  : x( other.x ) {}
};

// -----------------------------------------------------------------------
// status_value<>

CASE( "status_value<>: Disallows default construction" )
{
#if nssv_CONFIG_CONFIRMS_COMPILATION_ERRORS
    status_value<int, int> sv;
#endif
    EXPECT( !!"Avoid warning" );
}

CASE( "status_value<>: Allows construction from only status" )
{
    status_value<int, int> sv( 7 );
    
    EXPECT( sv.status() == 7 );
}

CASE( "status_value<>: Allows construction from status and non-default-constructible value" )
{
#if nssv_CONFIG_CONFIRMS_COMPILATION_ERRORS
    not_default_constructible x;
#endif
    status_value<int, not_default_constructible> sv( 7 );

    EXPECT( sv.status() == 7 );
}

CASE( "status_value<>: Allows construction from copied status and moved value" )
{
    status_value<int, move_constructible> sv( 7, move_constructible( 42 ) );

    EXPECT( sv.status()  ==  7 );
    EXPECT( sv.value().x == 42 );
}

CASE( "status_value<>: Allows construction from copied status and copied value" )
{
    copy_constructible x( 42 );
    status_value<int, copy_constructible> sv( 7, x );

    EXPECT( sv.status()  ==  7 );
    EXPECT( sv.value().x == 42 );
}

// A status_value may be moved. 
// A copy operation would make the type unusable for non-copyable 
// contained objects, so we do not provide a copy operation.

CASE( "status_value<>: Disallows copy-construction from other status_value of the same type" )
{
#if nssv_CONFIG_CONFIRMS_COMPILATION_ERRORS
    status_value<int, copy_constructible> sv1( 7, copy_constructible( 42 ) );
    status_value<int, copy_constructible> sv2( sv1 );    
#endif
    EXPECT( !!"Avoid warning" );
}

CASE( "status_value<>: Allows move-construction from other status_value of the same type" )
{
    status_value<int, move_constructible> sv1( 7, move_constructible( 42 ) );
    status_value<int, move_constructible> sv2( std::move( sv1 ) );    

    EXPECT( ! sv1 );
    EXPECT( sv2.status()  ==  7 );
    EXPECT( sv2.value().x == 42 );
}

// They may be queried for status. The design assumes that inlining 
// will remove the cost of returning a reference for cheap copyable types.

CASE( "status_value<>: Allows to observe its status" )
{
    status_value<int, int> sv( 7 );
    
    EXPECT( sv.status() == 7 );
}

// They may be queried for whether or not they have a value.

CASE( "status_value<>: Allows to observe the presence of a value (has_value())" )
{
    status_value<int, int> sv1( 7 );
    status_value<int, int> sv2( 7, 42 );
    
    EXPECT(  ! sv1.has_value() );
    EXPECT( !! sv2.has_value() );
}

CASE( "status_value<>: Allows to observe the presence of a value (operator bool)" )
{
    status_value<int, int> sv1( 7 );
    status_value<int, int> sv2( 7, 42 );
        
    EXPECT(  ! sv1 );
    EXPECT( !! sv2 );
}

// They may provide access to their value. 
// If they have no value, an exception of type Status, 
// with the status value passed to the constructor, is thrown.

CASE( "status_value<>: Allows to observe its value" )
{
    status_value<int, int> sv( 7, 42 );
        
    EXPECT( sv.value() == 42 );
}

CASE( "status_value<>: Throws status when observing a non-present value" )
{
    status_value<int, int> sv( 7 );

    EXPECT_THROWS(      sv.value() );
    EXPECT_THROWS_AS(   sv.value(), std::logic_error );
    EXPECT_THROWS_AS(   sv.value(), bad_status_value_access<int> );

//  EXPECT_THROWS_WITH( sv.value(), 7 );

    try
    {
        sv.value();
    }
    catch ( bad_status_value_access<int> const & e )
    {
        EXPECT( e.status() == 7 );
    }
}

// -----------------------------------------------------------------------
// test driver:

int main( int argc, char * argv[] )
{
    return lest::run( specification, argc, argv );
}

#if 0
//  
cl -nologo -W3   -wd4814 -EHsc -Dnssv_CONFIG_CONFIRMS_COMPILATION_ERRORS=0 -Dlest_FEATURE_AUTO_REGISTER=1 -I../include/nonstd status_value.t.cpp && status_value.t --pass
cl -nologo -Wall -wd4814 -EHsc -Dnssv_CONFIG_CONFIRMS_COMPILATION_ERRORS=0 -Dlest_FEATURE_AUTO_REGISTER=1 -I../include/nonstd status_value.t.cpp && status_value.t --pass

g++ -Wall -Wextra -std=c++03 -Wno-unused-parameter -Dlest_FEATURE_AUTO_REGISTER=1 -I../include/nonstd -o status_value.t.exe status_value.t.cpp && status_value.t --pass
g++ -Wall -Wextra -std=c++11 -Wno-unused-parameter -Dlest_FEATURE_AUTO_REGISTER=1 -I../include/nonstd -o status_value.t.exe status_value.t.cpp && status_value.t --pass
g++ -Wall -Wextra -std=c++14 -Wno-unused-parameter -Dlest_FEATURE_AUTO_REGISTER=1 -I../include/nonstd -o status_value.t.exe status_value.t.cpp && status_value.t --pass
g++ -Wall -Wextra -std=c++1y -Wno-unused-parameter -Dlest_FEATURE_AUTO_REGISTER=1 -I../include/nonstd -o status_value.t.exe status_value.t.cpp && status_value.t --pass

#endif // 0
