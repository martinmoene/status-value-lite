status_value - a class for status and optional value for C++11 and later&ensp;[![Build Status](https://travis-ci.org/martinmoene/status_value.png?branch=master)](https://travis-ci.org/martinmoene/status_value)
------------------------------------------------------------------------
status_value is a single-file header-only library for objects that represent a status and an optional value. It is intended for use with C++11 and later. The library is based on the proposal for status_value [1].

**Contents**  
- [Example usage](#example-usage)
- [In a nutshell](#in-a-nutshell)
- [License](#license)
- [Dependencies](#dependencies)
- [Installation](#installation)
- [Synopsis](#synopsis)
- [Comparison with like types](#comparison)
- [Reported to work with](#reported-to-work-with)
- [Implementation notes](#implementation-notes)
- [Notes and references](#notes-and-references)
- [Appendix](#appendix)


Example usage
-------------
```C++
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
```
### Compile and run
```
prompt> g++ -std=c++14 -Wall -I../include/nonstd -o 01-basic.exe 01-basic.cpp && 01-basic.exe 123 && 01-basic.exe abc
'123' is 123, Error: 'abc' isn't a number
```

In a nutshell
-------------
**status_value** is a single-file header-only library to represent objects that contain a status and an optional value. The library is an implementation of the  proposal for [std:&#58;status_value](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2014/n4233.html) [1,2] for use with C++11 and later.

**Features and properties of status_value** are ease of installation (single header), construction of a status, or a status and a value from a value that is convertible to the underlying type, move-construction from another status_value of the same type, testing for the presence of a value, operators and method value() for *checked* access to the value and access to the status.  

**Not provided** are copy-construction of a status_value (by design), ... .

For more examples, see [1].


License
-------
*status_value* uses the [MIT](LICENSE) license.


Dependencies
------------
*status_value* has no other dependencies than the [C++ standard library](http://en.cppreference.com/w/cpp/header).


Installation
------------

*status_value* is a single-file header-only library. Put `status_value.hpp` directly into the project source tree or somewhere reachable from your project.


Synopsis
--------

**Contents**  
- [Configuration macros](#configuration-macros)
- [Interface of status_value](#interface-of-status_value)  

### Configuration macros

\-D<b>nsel\_CONFIG\_CONFIRMS\_COMPILATION\_ERRORS</b>=0  
Define this macro to 1 to experience the by-design compile-time errors of the library in the test suite. Default is 0.


### Interface of status_value

| Kind         | Method                                                       | Result |
|--------------|--------------------------------------------------------------|--------|
| Construction | status_value() = delete                                      | disallow default construction |
| &nbsp;       | status_value( status_value && other )                        | move-construct from other |
| &nbsp;       | status_value( status_type const & s )                        | copy-construct from status |
| &nbsp;       | status_value( status_type const & s, value_type && v )       | copy-construct from status,<br>move construct from value |
| &nbsp;       | status_value(  status_type const & s, value_type const & v ) | copy-construct from status and value |
| Destruction  | ~status_value()                                              | status, value destroyed if present|
| Observers    | operator bool() const                                        | true if contains value |
| &nbsp;       | bool has_value() const                                       | true if contains value |
| &nbsp;       | status_type const & status() const                           | the status |
| &nbsp;       | value_type const & value() const                             | the value (const ref);<br>see [note 1](#note1) |
| &nbsp;       | value_type & value()                                         | the value (non-const ref);<br>see [note 1](#note1) |
| &nbsp;       | value_type const & operator *() const                        | the value (const ref);<br>see [note 1](#note1) |
| &nbsp;       | value_type & operator *()                                    | the value (non-const ref);<br>see [note 1](#note1) |

<a id="note1"></a>Note 1: checked access: if no content, throws status value.


<a id="comparison"></a>
Comparison with like types
--------------------------

|Feature               |std::optional |std::expected |nonstd::status_value |
|----------------------|--------------|--------------|---------------------|
|More information      | see [4]      | see [3]      | this work           |


Reported to work with
---------------------


Implementation notes
--------------------


Notes and references
--------------------

[1] Lawrence Crowl and Chris Mysen. [N4233 - A Class for Status and Optional Value](http://wg21.link/n4233). 10 October 2014.

[2] Lawrence Crowl. [P0157R0 - Handling Disappointment in C++](http://wg21.link/p0157r0). 7 July 2015.

[3] Vicente J. Botet Escriba. [Dxxxxr0 - A proposal to add a utility class to represent expected monad (Revision 2)](https://github.com/viboes/std-make/blob/master/doc/proposal/expected/DXXXXR0_expected.pdf) (PDF). 12 March 2016.

[4] Fernando Cacciola and Andrzej Krzemieński. [N3793 - A proposal to add a utility class to represent optional objects (Revision 5)](http://wg21.link/n3793). ISO/IEC JTC1 SC22 WG21 N3672 2013-04-19.  


Appendix
--------
### A.1 status_value test specification

```
status_value<>: Disallows default construction
status_value<>: Allows construction from only status
status_value<>: Allows construction from status and non-default-constructible value
status_value<>: Allows construction from copied status and moved value
status_value<>: Allows construction from copied status and copied value
status_value<>: Disallows copy-construction from other status_value of the same type
status_value<>: Allows move-construction from other status_value of the same type
status_value<>: Allows to observe its status
status_value<>: Allows to observe the presence of a value (has_value())
status_value<>: Allows to observe the presence of a value (operator bool)
status_value<>: Allows to observe its value
status_value<>: Throws status when observing a non-present value
```
