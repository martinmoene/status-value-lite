// Copyright 2016-2022 by Martin Moene
//
// This version targets C++98 and later.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// status_value is based on:
//   A Class for Status and Optional Value, P0262r0
//   by Lawrence Crowl and Chris Mysen

#include nsstv_STATUS_VALUE_HEADER

#include "lest_cpp03.hpp"

#define CASE( name ) lest_CASE( specification, name )

static lest::tests specification;

using namespace nonstd;

#ifndef nsstsv_CONFIG_CONFIRMS_COMPILATION_ERRORS
#define nsstsv_CONFIG_CONFIRMS_COMPILATION_ERRORS  0
#endif

template< typename T >
void use( T const & ) {}

struct V { int i; V():i(42){} };

class not_default_constructible
{
    not_default_constructible();
};

#if nsstsv_CPP11_OR_GREATER

struct move_constructible
{
    int x;
    move_constructible( int x_ ) : x(x_) {}
    move_constructible( move_constructible && other ) : x( std::move( other.x ) ) {}
    move_constructible( move_constructible const & other ) = delete;
};
#endif

struct copy_constructible
{
    int x;
    copy_constructible( int x_ ) : x(x_) {}
    copy_constructible( copy_constructible const & other ) : x( other.x ) {}
};

// -----------------------------------------------------------------------
// status_value<>

CASE( "status_value<>: Disallows default construction" )
{
#if nsstsv_CONFIG_CONFIRMS_COMPILATION_ERRORS
    status_value<int, int> sv;
#endif
    EXPECT( "Avoid warning" );
}

CASE( "status_value<>: Allows construction from only status" )
{
    status_value<int, int> sv( 7 );

    EXPECT( sv.status() == 7 );
}

CASE( "status_value<>: Allows construction from status and non-default-constructible value" )
{
#if nsstsv_CONFIG_CONFIRMS_COMPILATION_ERRORS
    not_default_constructible x;
#endif
    status_value<int, not_default_constructible> sv( 7 );

    EXPECT( sv.status() == 7 );
}

CASE( "status_value<>: Allows construction from copied status and moved value (C++11)" )
{
#if nsstsv_CPP11_OR_GREATER
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

#if nsstsv_CPP11_OR_GREATER

CASE( "status_value<>: Disallows copy-construction from other status_value of the same type (C++11)" )
{
# if nsstsv_CONFIG_CONFIRMS_COMPILATION_ERRORS
    status_value<int, copy_constructible> sv1( 7, copy_constructible( 42 ) );
    status_value<int, copy_constructible> sv2( sv1 );
# endif
    EXPECT( "Avoid warning" );
}

#else

CASE( "status_value<>: Allows copy-construction from other status_value of the same type (pre C++11)" )
{
    status_value<int, copy_constructible> sv1( 7, copy_constructible( 42 ) );
    status_value<int, copy_constructible> sv2( sv1 );

    EXPECT( "Avoid warning" );
}

#endif

CASE( "status_value<>: Allows move-construction from other status_value of the same type (C++11)" )
{
#if nsstsv_CPP11_OR_GREATER
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

CASE( "status_value<>: Allows to observe its value (operator*)" )
{
    status_value<int, int>        sv( 7, 42 );
    status_value<int, int> const csv( 7, 42 );

    EXPECT(  *sv == 42 );
    EXPECT( *csv == 42 );

#if nsstsv_CPP11_OR_GREATER
    EXPECT( *std::move(  sv ) == 42 );
    EXPECT( *std::move( csv ) == 42 );
#endif
}

CASE( "status_value<>: Allows to observe its value (operator->)" )
{
    // struct V { int i = 42; } v;
    V v;
    status_value<int, V>        sv( 7, v );
    status_value<int, V> const csv( 7, v );

    EXPECT(  sv->i == 42 );
    EXPECT( csv->i == 42 );

#if nsstsv_CPP11_OR_GREATER
    EXPECT( std::move(  sv )->i == 42 );
    EXPECT( std::move( csv )->i == 42 );
#endif
}

CASE( "status_value<>: Throws when observing non-engaged (value())" )
{
#if ! nsstsv_CONFIG_NO_EXCEPTIONS
    SETUP("") {
        status_value<int, int>        sv( 7 );
        status_value<int, int> const csv( 7 );

    SECTION("for l-value reference")
    {
        EXPECT_THROWS(     sv.value() );
        EXPECT_THROWS(    csv.value() );

        EXPECT_THROWS_AS(  sv.value(), std::logic_error );
        EXPECT_THROWS_AS( csv.value(), std::logic_error );

        EXPECT_THROWS_AS(  sv.value(), bad_status_value_access<int> );
        EXPECT_THROWS_AS( csv.value(), bad_status_value_access<const int> );
    }

#if nsstsv_CPP11_OR_GREATER

    SECTION("for r-value reference (throws)")
    {
        EXPECT_THROWS( std::move(  sv ).value() );
        EXPECT_THROWS( std::move( csv ).value() );
    }
    SECTION("for r-value reference (throws-as)")
    {
        EXPECT_THROWS_AS( std::move(  sv ).value(), bad_status_value_access<int> );
        EXPECT_THROWS_AS( std::move( csv ).value(), bad_status_value_access<const int> );
    }

#endif // nsstsv_CPP11_OR_GREATER

    SECTION("throw with expected status value")
    {
        try { sv.value();  } catch ( bad_status_value_access<      int> const & e ) { EXPECT( e.status() == 7 ); }
        try { csv.value(); } catch ( bad_status_value_access<const int> const & e ) { EXPECT( e.status() == 7 ); }
    }}
#else
    EXPECT( !!"status_value: exceptions not available (nsstsv_CONFIG_NO_EXCEPTIONS)" );
#endif
}

CASE( "status_value<>: Throws when observing non-engaged (operator*())" )
{
#if ! nsstsv_CONFIG_NO_EXCEPTIONS
    SETUP("") {
        status_value<int, int>        sv( 7 );
        status_value<int, int> const csv( 7 );

    SECTION("for l-value reference")
    {
        EXPECT_THROWS(     *sv );
        EXPECT_THROWS(    *csv );

        EXPECT_THROWS_AS(  *sv, std::logic_error );
        EXPECT_THROWS_AS( *csv, std::logic_error );

        EXPECT_THROWS_AS(  *sv, bad_status_value_access<int> );
        EXPECT_THROWS_AS( *csv, bad_status_value_access<const int> );
    }

#if nsstsv_CPP11_OR_GREATER

    SECTION("for r-value reference (throws)")
    {
        EXPECT_THROWS( *std::move(  sv ) );
        EXPECT_THROWS( *std::move( csv ) );
    }
    SECTION("for r-value reference (throws-as)")
    {
        EXPECT_THROWS_AS( *std::move(  sv ), bad_status_value_access<int> );
        EXPECT_THROWS_AS( *std::move( csv ), bad_status_value_access<const int> );
    }

#endif // nsstsv_CPP11_OR_GREATER

    SECTION("throw with expected status value")
    {
        try { *sv;  } catch ( bad_status_value_access<      int> const & e ) { EXPECT( e.status() == 7 ); }
        try { *csv; } catch ( bad_status_value_access<const int> const & e ) { EXPECT( e.status() == 7 ); }
    }}
#else
    EXPECT( !!"status_value: exceptions not available (nsstsv_CONFIG_NO_EXCEPTIONS)" );
#endif
}

CASE( "status_value<>: Throws when observing non-engaged (operator->())" )
{
#if ! nsstsv_CONFIG_NO_EXCEPTIONS
    SETUP("") {
        // struct V { int i = 42; } v;
        status_value<int, V>        sv( 7 );
        status_value<int, V> const csv( 7 );

    SECTION("for l-value reference")
    {
        EXPECT_THROWS(     sv->i );
        EXPECT_THROWS(    csv->i );

        EXPECT_THROWS_AS(  sv->i, std::logic_error );
        EXPECT_THROWS_AS( csv->i, std::logic_error );

        EXPECT_THROWS_AS(  sv->i, bad_status_value_access<int> );
        EXPECT_THROWS_AS( csv->i, bad_status_value_access<const int> );

//      EXPECT_THROWS_WITH(     sv->i, 7 );
//      EXPECT_THROWS_WITH(    csv->i, 7 );

    }

#if nsstsv_CPP11_OR_GREATER

    SECTION("for r-value reference (throws)")
    {
        EXPECT_THROWS( std::move(  sv )->i );
        EXPECT_THROWS( std::move( csv )->i );

//      EXPECT_THROWS_WITH( std::move(  sv )->i, 7 );
//      EXPECT_THROWS_WITH( std::move( csv )->i, 7 );

    }
    SECTION("for r-value reference (throws-as)")
    {
        EXPECT_THROWS_AS( std::move(  sv )->i, bad_status_value_access<int> );
        EXPECT_THROWS_AS( std::move( csv )->i, bad_status_value_access<const int> );

    //  EXPECT_THROWS_WITH( std::move(  sv )->i, 7 );
    //  EXPECT_THROWS_WITH( std::move( csv )->i, 7 );

    }

#endif // nsstsv_CPP11_OR_GREATER

    SECTION("throw with expected status value")
    {
        try { use( sv->i  ); } catch ( bad_status_value_access<      int> const & e ) { EXPECT( e.status() == 7 ); }
        try { use( csv->i ); } catch ( bad_status_value_access<const int> const & e ) { EXPECT( e.status() == 7 ); }
    }}
#else
    EXPECT( !!"status_value: exceptions not available (nsstsv_CONFIG_NO_EXCEPTIONS)" );
#endif
}

// -----------------------------------------------------------------------
// Tests that print information:

struct S{ S(){} };

#if !nsstsv_CONFIG_MAX_ALIGN_HACK

#define nsstsv_OUTPUT_ALIGNMENT_OF( type ) \
    "alignment_of<" #type ">: " <<  \
     alignment_of<type>::value  << "\n" <<

CASE("Show alignment of various types" "[.]" )
{
#if nsstsv_CPP11_OR_GREATER
    using std::alignment_of;
#elif nsstsv_COMPILER_IS_VC6
    using namespace ::nonstd::status_value_detail;
#else
    using ::nonstd::status_value_detail::alignment_of;
#endif
    std::cout <<
        nsstsv_OUTPUT_ALIGNMENT_OF( char )
        nsstsv_OUTPUT_ALIGNMENT_OF( short )
        nsstsv_OUTPUT_ALIGNMENT_OF( int )
        nsstsv_OUTPUT_ALIGNMENT_OF( long )
        nsstsv_OUTPUT_ALIGNMENT_OF( float )
        nsstsv_OUTPUT_ALIGNMENT_OF( double )
        nsstsv_OUTPUT_ALIGNMENT_OF( long double )
        nsstsv_OUTPUT_ALIGNMENT_OF( S )
         "";

    EXPECT( "Avoid warning" );
}
#undef nsstsv_OUTPUT_ALIGNMENT_OF
#endif

#define nsstsv_OUTPUT_SIZEOF( type ) \
    "sizeof( status_value<char," #type "> ): " << \
     sizeof( status_value<char,   type>   )    << " (" << sizeof(type) << ")\n" <<

CASE("Show sizeof various status_value types" "[.]" )
{
    std::cout <<
        "sizeof( nonstd::status_value_detail::storage_t<char, char> ): " <<
         sizeof( nonstd::status_value_detail::storage_t<char, char> )    << "\n" <<
         nsstsv_OUTPUT_SIZEOF( char )
         nsstsv_OUTPUT_SIZEOF( short )
         nsstsv_OUTPUT_SIZEOF( int )
         nsstsv_OUTPUT_SIZEOF( long )
         nsstsv_OUTPUT_SIZEOF( float )
         nsstsv_OUTPUT_SIZEOF( double )
         nsstsv_OUTPUT_SIZEOF( long double )
         nsstsv_OUTPUT_SIZEOF( S )
         "";

    EXPECT( "Avoid warning" );
}
#undef nsstsv_OUTPUT_SIZEOF

// -----------------------------------------------------------------------
// test driver:

int main( int argc, char * argv[] )
{
    return lest::run( specification, argc, argv );
}

#if 0
//
cl -nologo -W3   -wd4814 -EHsc -Dnsstsv_CONFIG_CONFIRMS_COMPILATION_ERRORS=0 -Dlest_FEATURE_AUTO_REGISTER=1 -I../include status_value_cpp98.t.cpp && status_value_cpp98.t --pass
cl -nologo -Wall -wd4814 -EHsc -Dnsstsv_CONFIG_CONFIRMS_COMPILATION_ERRORS=0 -Dlest_FEATURE_AUTO_REGISTER=1 -I../include status_value_cpp98.t.cpp && status_value_cpp98.t --pass

g++ -Wall -Wextra -std=c++03 -Wno-unused-parameter -Dlest_FEATURE_AUTO_REGISTER=1 -I../include -o status_value_cpp98.t.exe status_value_cpp98.t.cpp && status_value_cpp98.t --pass
g++ -Wall -Wextra -std=c++11 -Wno-unused-parameter -Dlest_FEATURE_AUTO_REGISTER=1 -I../include -o status_value_cpp98.t.exe status_value_cpp98.t.cpp && status_value_cpp98.t --pass
g++ -Wall -Wextra -std=c++14 -Wno-unused-parameter -Dlest_FEATURE_AUTO_REGISTER=1 -I../include -o status_value_cpp98.t.exe status_value_cpp98.t.cpp && status_value_cpp98.t --pass
g++ -Wall -Wextra -std=c++1y -Wno-unused-parameter -Dlest_FEATURE_AUTO_REGISTER=1 -I../include -o status_value_cpp98.t.exe status_value_cpp98.t.cpp && status_value_cpp98.t --pass

#endif // 0
