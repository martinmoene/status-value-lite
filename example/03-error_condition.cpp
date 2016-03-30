// Convert text to number and yield status_value with number and error_condition.

#include "status_value.hpp"

#include <cstdlib>
#include <iostream>
#include <string>           // required by VC14 (VS2015)
#include <system_error>
		
using namespace nonstd;

auto to_int( char const * const text ) -> status_value<std::error_condition, int> 
{
    char * pos = nullptr;
    auto value = strtol( text, &pos, 0 );

    if ( pos != text ) return { std::error_condition(), value };
    else               return { std::make_error_condition( std::errc::invalid_argument ) };
}

int main( int argc, char * argv[] )
{
    auto text = argc > 1 ? argv[1] : "42";

    auto svi = to_int( text );

    if ( svi ) std::cout << svi.status().message() << ": '" << text << "' is " << *svi << ", ";
    else       std::cout << "Error: " << svi.status().message();
}

// cl -EHsc -wd4814 -I../include/nonstd 03-error_condition.cpp && 03-error_condition.exe 123 && 03-error_condition.exe abc
// g++ -std=c++14 -Wall -I../include/nonstd -o 03-error_condition.exe 03-error_condition.cpp && 03-error_condition.exe 123 && 03-error_condition.exe abc
// No error: '123' is 123, Error: Invalid argument
