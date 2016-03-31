// Convert text to number and yield status_value with number and error_condition.

#include "status_value_cpp98.hpp"

#include <cstdlib>
#include <iostream>
#include <string>

using namespace nonstd;

status_value< std::string, int >
to_int( char const * const text ) 
{
    char * pos = NULL;
    long value = strtol( text, &pos, 0 );

    if ( pos != text ) return status_value< std::string, int >( "Excellent", value );
    else               return status_value< std::string, int >( std::string("'") + text + "' isn't a number" );
}

int main( int argc, char * argv[] )
{
    char const * const text = argc > 1 ? argv[1] : "42";

    status_value<std::string, int> svi = to_int( text );

    if ( svi ) std::cout << svi.status() << ": '" << text << "' is " << *svi << ", ";
    else       std::cout << "Error: " << svi.status();

    return 0; // VC6
}

// cl -EHsc -wd4814 -I../include/nonstd 01-basic_cpp98.cpp && 01-basic_cpp98.exe 123 && 01-basic_cpp98.exe abc
// g++ -std=c++98 -Wall -I../include/nonstd -o 01-basic_cpp98.exe 01-basic_cpp98.cpp && 01-basic_cpp98.exe 123 && 01-basic_cpp98.exe abc
// Excellent: '123' is 123, Error: 'abc' isn't a number
