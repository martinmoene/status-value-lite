// Convert text to number and yield expected with number or error text.

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

    if ( pos != text ) return { "excellent", value };
    else               return { "'"s + text + "' isn't a number" };
}

int main( int argc, char * argv[] )
{
    auto text = argc > 1 ? argv[1] : "42";

    auto ei = to_int( text );

    if ( ei ) std::cout << "'" << text << "' is " << *ei << ", ";
    else      std::cout << "Error: " << ei.status();
}

// cl -EHsc -wd4814 -I../include/nonstd 01-basic.cpp && 01-basic.exe 123 && 01-basic.exe abc
// g++ -std=c++14 -Wall -I../include/nonstd -o 01-basic.exe 01-basic.cpp && 01-basic.exe 123 && 01-basic.exe abc
// '123' is 123, Error: 'abc' isn't a number
