// Copyright 2016-2018 by Martin Moene
//
// This version targets C++98 and later.
//
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// status_value is based on:
//   A Class for Status and Optional Value, P0262r0
//   by Lawrence Crowl and Chris Mysen

#include "status_value_cpp98.hpp"

#include "lest_cpp03.hpp"

#define CASE( name ) lest_CASE( specification, name )

static lest::tests specification;

using namespace nonstd;

class not_default_constructible
{
    not_default_constructible();
};

#if nssv_CPP11_OR_GREATER

struct move_constructible
{
    int x;
    move_constructible( int x ) : x(x) {}
    move_constructible( move_constructible && other ) : x( std::move( other.x ) ) {}
    move_constructible( move_constructible const & other ) = delete;
};
#endif 

struct copy_constructible
{
    int x;
    copy_constructible( int x ) : x(x) {}
    copy_constructible( copy_constructible const & other ) : x( other.x ) {}
};

// -----------------------------------------------------------------------
// status_value<>

CASE( "status_value<>: Disallows default construction" )
{
#if nssv_CONFIG_CONFIRMS_COMPILATION_ERRORS
    status_value<int, int> sv;
#endif
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

CASE( "status_value<>: Allows construction from copied status and moved value (C++11)" )
{
#if nssv_CPP11_OR_GREATER
    status_value<int, move_constructible> sv( 7, move_constructible( 42 ) );

    EXPECT( sv.status()  ==  7 );
    EXPECT( sv.value().x == 42 );
#else
    EXPECT( !!"No move-construction from value (pre C++11)");
#endif
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

#if nssv_CPP11_OR_GREATER

CASE( "status_value<>: Disallows copy-construction from other status_value of the same type (C++11)" )
{
# if nssv_CONFIG_CONFIRMS_COMPILATION_ERRORS
    status_value<int, copy_constructible> sv1( 7, copy_constructible( 42 ) );
    status_value<int, copy_constructible> sv2( sv1 );    
# endif
}
#else
CASE( "status_value<>: Allows copy-construction from other status_value of the same type (pre C++11)" )
{
    status_value<int, copy_constructible> sv1( 7, copy_constructible( 42 ) );
    status_value<int, copy_constructible> sv2( sv1 );    
}

#endif

CASE( "status_value<>: Allows move-construction from other status_value of the same type (C++11)" )
{
#if nssv_CPP11_OR_GREATER
    status_value<int, move_constructible> sv1( 7, move_constructible( 42 ) );
    status_value<int, move_constructible> sv2( std::move( sv1 ) );    

    EXPECT( ! sv1 );
    EXPECT( sv2.status()  ==  7 );
    EXPECT( sv2.value().x == 42 );
#else
    EXPECT( !!"No move-construction from otherstatus_value (pre C++11)");
#endif
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
    EXPECT_THROWS_AS(   sv.value(), int );

//  EXPECT_THROWS_WITH( sv.value(), 7 );

    try
    {
        sv.value();
    }
    catch ( int const & e )
    {
        EXPECT( e == 7 );
    }
}

// -----------------------------------------------------------------------
// Tests that print information:

struct S{ S(){} };

#if !nssv_CONFIG_MAX_ALIGN_HACK

#define nssv_OUTPUT_ALIGNMENT_OF( type ) \
    "alignment_of<" #type ">: " <<  \
     alignment_of<type>::value  << "\n" <<

CASE("Show alignment of various types" "[.]" )
{
#if nssv_CPP11_OR_GREATER  
    using std::alignment_of;
#elif nssv_COMPILER_IS_VC6
    using namespace ::nonstd::status_value_detail;
#else
    using ::nonstd::status_value_detail::alignment_of;
#endif    
    std::cout << 
        nssv_OUTPUT_ALIGNMENT_OF( char )
        nssv_OUTPUT_ALIGNMENT_OF( short )
        nssv_OUTPUT_ALIGNMENT_OF( int )
        nssv_OUTPUT_ALIGNMENT_OF( long )
        nssv_OUTPUT_ALIGNMENT_OF( float )
        nssv_OUTPUT_ALIGNMENT_OF( double )
        nssv_OUTPUT_ALIGNMENT_OF( long double )
        nssv_OUTPUT_ALIGNMENT_OF( S )
         "";
}
#undef nssv_OUTPUT_ALIGNMENT_OF
#endif

#define nssv_OUTPUT_SIZEOF( type ) \
    "sizeof( status_value<char," #type "> ): " << \
     sizeof( status_value<char,   type>   )    << " (" << sizeof(type) << ")\n" <<

CASE("Show sizeof various status_value types" "[.]" )
{
    std::cout << 
        "sizeof( nonstd::status_value_detail::storage_t<char, char> ): " << 
         sizeof( nonstd::status_value_detail::storage_t<char, char> )    << "\n" << 
         nssv_OUTPUT_SIZEOF( char )
         nssv_OUTPUT_SIZEOF( short )
         nssv_OUTPUT_SIZEOF( int )
         nssv_OUTPUT_SIZEOF( long )
         nssv_OUTPUT_SIZEOF( float )
         nssv_OUTPUT_SIZEOF( double )
         nssv_OUTPUT_SIZEOF( long double )
         nssv_OUTPUT_SIZEOF( S )
         "";
}
#undef nssv_OUTPUT_SIZEOF

// -----------------------------------------------------------------------
// test driver:

int main( int argc, char * argv[] )
{
    return lest::run( specification, argc, argv );
}

#if 0
//  
cl -nologo -W3   -wd4814 -EHsc -Dnssv_CONFIG_CONFIRMS_COMPILATION_ERRORS=0 -Dlest_FEATURE_AUTO_REGISTER=1 -I../include/nonstd status_value_cpp98.t.cpp && status_value_cpp98.t --pass
cl -nologo -Wall -wd4814 -EHsc -Dnssv_CONFIG_CONFIRMS_COMPILATION_ERRORS=0 -Dlest_FEATURE_AUTO_REGISTER=1 -I../include/nonstd status_value_cpp98.t.cpp && status_value_cpp98.t --pass

g++ -Wall -Wextra -std=c++03 -Wno-unused-parameter -Dlest_FEATURE_AUTO_REGISTER=1 -I../include/nonstd -o status_value_cpp98.t.exe status_value_cpp98.t.cpp && status_value_cpp98.t --pass
g++ -Wall -Wextra -std=c++11 -Wno-unused-parameter -Dlest_FEATURE_AUTO_REGISTER=1 -I../include/nonstd -o status_value_cpp98.t.exe status_value_cpp98.t.cpp && status_value_cpp98.t --pass
g++ -Wall -Wextra -std=c++14 -Wno-unused-parameter -Dlest_FEATURE_AUTO_REGISTER=1 -I../include/nonstd -o status_value_cpp98.t.exe status_value_cpp98.t.cpp && status_value_cpp98.t --pass
g++ -Wall -Wextra -std=c++1y -Wno-unused-parameter -Dlest_FEATURE_AUTO_REGISTER=1 -I../include/nonstd -o status_value_cpp98.t.exe status_value_cpp98.t.cpp && status_value_cpp98.t --pass

#endif // 0
