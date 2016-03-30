// Convert text to number and yield status_value with number and error_condition.

#include "status_value.hpp"

#include <cstdlib>
#include <iostream>
#include <string>

using namespace nonstd;
using namespace std::literals;

auto to_int( char const * const text ) -> status_value<std::string, int> 
{
    char * pos = nullptr;
    auto value = strtol( text, &pos, 0 );

    if ( pos != text ) return { "Excellent", value };
    else               return { "'"s + text + "' isn't a number" };
}

int main( int argc, char * argv[] )
{
    auto text = argc > 1 ? argv[1] : "42";

    auto svi = to_int( text );

    if ( svi ) std::cout << svi.status() << ": '" << text << "' is " << *svi << ", ";
    else       std::cout << "Error: " << svi.status();
}

// cl -EHsc -wd4814 -I../include/nonstd 01-basic.cpp && 01-basic.exe 123 && 01-basic.exe abc
// g++ -std=c++14 -Wall -I../include/nonstd -o 01-basic.exe 01-basic.cpp && 01-basic.exe 123 && 01-basic.exe abc
// Excellent: '123' is 123, Error: 'abc' isn't a number
