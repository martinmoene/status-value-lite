status_value - a class for status and optional value for C++11 and later
------------------------------------------------------------------------
[![Language](https://img.shields.io/badge/language-C++-blue.svg)](https://isocpp.org/)  [![Standard](https://img.shields.io/badge/c%2B%2B-11-blue.svg)](https://en.wikipedia.org/wiki/C%2B%2B#Standardization) [![Standard](https://img.shields.io/badge/c%2B%2B-98-orange.svg)](https://en.wikipedia.org/wiki/C%2B%2B#Standardization) [![License](https://img.shields.io/badge/license-MIT-blue.svg)](https://opensource.org/licenses/MIT) [![Build Status](https://travis-ci.org/martinmoene/status_value.svg?branch=master)](https://travis-ci.org/martinmoene/status_value)  [![Version](https://badge.fury.io/gh/martinmoene%2Fstatus_value.svg)](https://github.com/martinmoene/status_value/releases) [![download](https://img.shields.io/badge/latest%20version%20%20-download-blue.svg)](https://raw.githubusercontent.com/martinmoene/status_value/master/include/nonstd/status_value.hpp)

status_value is a single-file header-only library for objects that represent a status and an optional value. It is intended for use with C++11 and later. There is a [separate variant](README_cpp98.md) for use with C++98 and later. The library is based on the proposal for status_value [[1](#ref1)].

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
```
### Compile and run
```
prompt> g++ -std=c++14 -Wall -I../include/nonstd -o 01-basic.exe 01-basic.cpp && 01-basic.exe 123 && 01-basic.exe abc
Excellent: '123' is 123, Error: 'abc' isn't a number
```

In a nutshell
-------------
**status_value** is a single-file header-only library to represent objects that contain a status and an optional value. The library is an implementation of the  proposal for [std:&#58;status_value](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2014/n4233.html) [[1](#ref1),[2](#ref2)] for use with C++11 and later.

**Features and properties of status_value** are ease of installation (single header), construction of a status, or a status and a value from a value that is convertible to the underlying type, move-construction from another status_value of the same type, testing for the presence of a value, operators and method value() for *checked* access to the value and access to the status.  

**Not provided** are copy-construction of a status_value (by design), ... .

For more examples, see [[1](#ref1)].


License
-------
*status_value* is distributed under the [Boost Software License](https://github.com/martinmoene/span-lite/blob/master/LICENSE.txt).


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

\-D<b>nssv\_CONFIG\_NO\_EXCEPTIONS</b>=0  
Define this to 1 if you want to compile without exceptions. If not defined, the header tries and detect if exceptions have been disabled (e.g. via -fno-exceptions). Disabling exceptions will force contract violation to call `std::terminate()`. Default is 0.

\-D<b>nssv\_CONFIG\_CONFIRMS\_COMPILATION\_ERRORS</b>=0  
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

|Feature               |std::optional     |std::expected     |nonstd::status_value |
|----------------------|------------------|------------------|---------------------|
|More information      | see [[4](#ref4)] | see [[3](#ref3)] | this work           |
|                      |                  |                  |                     |
| C++98                | &ndash;&ensp;[note 2](#note2) | &ndash; | &ndash;&ensp;[note 4](#note4)  |
| C++11                | &ndash;          | &ndash;&ensp;[note 3](#note3) | &#10003; |
| C++14                | &ndash;          | &ndash;          | &#10003;            |
| C++17                | &#10003;         | &ndash;          | &#10003;            |
|                      |                  |                  |                     |
|DefaultConstructible  | &#10003;         | &#10003;         | &#10003;            |
|In-place construction | &#10003;         | &#10003;         | &ndash;             |
|Literal type          | &#10003;         | &#10003;         | &ndash;             |
|                      |                  |                  |                     |
|Disengaged information| &ndash;          | &#10003;         | &#10003;&ensp;status|
|Vary disengaged type  | &ndash;          | &#10003;         | &#10003;&ensp;status|
|Engaged nonuse throws | &ndash;          | &ndash;          | &ndash;             |
|Disengaged use throws | &#10003;&ensp;value() | &#10003;&ensp;value() | &#10003;&ensp;all |
|                      |                  |                  |                     |
|Proxy (rel.ops)       | &#10003;         | &#10003;         | &ndash;             |
|References            | &#10003;         | &ndash;          | &ndash;             |
|Chained visitor(s)    | &ndash;          | &#10003;         | &ndash;             |

<a id="note2"></a>Note 2: [optional lite](https://github.com/martinmoene/optional-lite) - Optional (nullable) objects for C++98 and later.  
<a id="note3"></a>Note 3: [expected lite](https://github.com/martinmoene/expected-lite) - Expected objects for C++11 and later.  
<a id="note4"></a>Note 4: This project provides a [variant of status_value](README_cpp98.md) for use with C++98 and later.


Reported to work with
---------------------
*status_value* is reported to work with the following compilers: 
- Visual VC14 (VS2015)
- GNUC 5.2.0 with -std=c++11, -std=c++14, -std=c++1y 
- clang 3.6, 3.7 with -std=c++11, -std=c++14 (on Travis)


Implementation notes
--------------------


Notes and references
--------------------
<a id="ref1"></a>[1] Lawrence Crowl and Chris Mysen. [p0262 - A Class for Status and Optional Value (latest)](http://wg21.link/p0262), 14 February 2016. [N4233](http://wg21.link/n4233), 10 October 2014.

<a id="ref2"></a>[2] Lawrence Crowl. [p0157 - Handling Disappointment in C++ (latest)](http://wg21.link/p0157), 7 July 2015.

<a id="ref3"></a>[3] Vicente J. Botet Escriba. [p0323 - A proposal to add a utility class to represent expected object (latest)](http://wg21.link/p0323) (PDF). ([r6](http://wg21.link/p0323r6), [r5](http://wg21.link/p0323r5), [r4](http://wg21.link/p0323r4), [r3](http://wg21.link/p0323r3), [r2](http://wg21.link/p0323r2), [r1](http://wg21.link/n4109), [r0](http://wg21.link/n4015), [draft](https://github.com/viboes/std-make/blob/master/doc/proposal/expected/DXXXXR0_expected.pdf)).

<a id="ref4"></a>[4] Fernando Cacciola and Andrzej Krzemieński. [N3793 - A proposal to add a utility class to represent optional objects (Revision 5)](http://wg21.link/n3793). 03 October 2013. [N3672 r4](http://wg21.link/n3672), [N3527 r3](http://wg21.link/n3527), [N3406 r2](http://wg21.link/n3406), [N1878 r1](http://wg21.link/n1878), [N3966 - Fixes for optional objects](http://wg21.link/n3966).


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
