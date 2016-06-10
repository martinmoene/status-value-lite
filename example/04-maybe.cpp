// Create a maybe type.

#include "status_value.hpp"

#include <iostream>

template< typename T >
struct maybe : nonstd::status_value<int, T>
{
    maybe(     ) : nonstd::status_value<int, T>( 0    ) {};
    maybe( T v ) : nonstd::status_value<int, T>( 0, v ) {};
};

auto value() -> maybe<int> { return { 42 }; }
auto empty() -> maybe<int> { return {    }; }

void print( char const * text, maybe<int> value )
{
    if ( value ) std::cout << text << ": contents: " << *value << "\n";
    else         std::cout << text << ": no contents\n" ;    
}

int main( int argc, char * argv[] )
{
    print( "value()", value() );
    print( "empty()", empty() );
}

// cl -EHsc -wd4814 -I../include/nonstd 04-maybe.cpp && 04-maybe.exe
// g++ -std=c++11 -Wall -I../include/nonstd -o 04-maybe.exe 04-maybe.cpp && 04-maybe.exe
// value(): contents: 42
// empty(): no contents
