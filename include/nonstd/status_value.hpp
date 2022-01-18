// Copyright 2016-2022 by Martin Moene
//
// This version targets C++11 and later.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// status_value is based on:
//   A Class for Status and Optional Value, P0262r0
//   by Lawrence Crowl and Chris Mysen

#ifndef NONSTD_STATUS_VALUE_HPP
#define NONSTD_STATUS_VALUE_HPP

#include <utility>

#define status_value_MAJOR  1
#define status_value_MINOR  2
#define status_value_PATCH  0

#define status_value_VERSION  nsstsv_STRINGIFY(status_value_MAJOR) "." nsstsv_STRINGIFY(status_value_MINOR) "." nsstsv_STRINGIFY(status_value_PATCH)

#define nsstsv_STRINGIFY(  x )  nsstsv_STRINGIFY_( x )
#define nsstsv_STRINGIFY_( x )  #x

// Control presence of exception handling (try and auto discover):

#ifndef nsstsv_CONFIG_NO_EXCEPTIONS
# if defined(_MSC_VER)
#  include <cstddef>    // for _HAS_EXCEPTIONS
# endif
# if defined(__cpp_exceptions) || defined(__EXCEPTIONS) || (_HAS_EXCEPTIONS)
#  define nsstsv_CONFIG_NO_EXCEPTIONS  0
# else
#  define nsstsv_CONFIG_NO_EXCEPTIONS  1
# endif
#endif

// C++ language version detection (C++20 is speculative):
// Note: VC14.0/1900 (VS2015) lacks too much from C++14.

#ifndef   nsstsv_CPLUSPLUS
# if defined(_MSVC_LANG ) && !defined(__clang__)
#  define nsstsv_CPLUSPLUS  (_MSC_VER == 1900 ? 201103L : _MSVC_LANG )
# else
#  define nsstsv_CPLUSPLUS  __cplusplus
# endif
#endif

#define nsstsv_CPP98_OR_GREATER  ( nsstsv_CPLUSPLUS >= 199711L )
#define nsstsv_CPP11_OR_GREATER  ( nsstsv_CPLUSPLUS >= 201103L )
#define nsstsv_CPP14_OR_GREATER  ( nsstsv_CPLUSPLUS >= 201402L )
#define nsstsv_CPP17_OR_GREATER  ( nsstsv_CPLUSPLUS >= 201703L )
#define nsstsv_CPP20_OR_GREATER  ( nsstsv_CPLUSPLUS >= 202000L )

#define nsstsv_CPP11_140  (nsstsv_CPP11_OR_GREATER || _MSC_VER >= 1900)
#define nsstsv_CPP14_000  (nsstsv_CPP14_OR_GREATER)
#define nsstsv_CPP17_000  (nsstsv_CPP17_OR_GREATER)

// Presence of C++ language features:

#define nsstsv_HAVE_CONSTEXPR_14   nsstsv_CPP14_000
#define nsstsv_HAVE_NOEXCEPT       nsstsv_CPP11_140
#define nsstsv_HAVE_NORETURN       nsstsv_CPP17_000

#if nsstsv_HAVE_CONSTEXPR_14
# define nsstsv_constexpr14 constexpr
#else
# define nsstsv_constexpr14 /*constexpr*/
#endif

#if nsstsv_HAVE_NOEXCEPT
# define nsstsv_noexcept noexcept
#else
# define nsstsv_noexcept /*noexcept*/
#endif

#if nsstsv_HAVE_NORETURN
# define nsstsv_noreturn [[noreturn]]
#else
# define nsstsv_noreturn /*[[noreturn]]*/
#endif

// Additional includes:

#if ! nsstsv_CONFIG_NO_EXCEPTIONS
# include <stdexcept>
#endif

namespace nonstd {

template< typename S, typename V >
class status_value;

namespace status_value_detail {

// Union to hold value:

template< typename S, typename V >
union storage_t
{
    friend class status_value<S,V>;

private:
    typedef V value_type;

    // no-op construction
    storage_t() nsstsv_noexcept {}
    ~storage_t() {}

    void construct_value( value_type const & v )
    {
        new( &m_value ) value_type( v );
    }

    void construct_value( value_type && v )
    {
        new( &m_value ) value_type( std::move( v ) );
    }

    void destruct_value() nsstsv_noexcept
    {
        m_value.~value_type();
    }

    constexpr value_type const & value() const & nsstsv_noexcept
    {
        return m_value;
    }

    value_type & value() & nsstsv_noexcept
    {
        return m_value;
    }

    constexpr value_type const && value() const &&
    {
        return std::move( m_value );
    }

    value_type && value() &&
    {
        return std::move( m_value );
    }

    value_type const * value_ptr() const  nsstsv_noexcept
    {
        return &m_value;
    }

    value_type * value_ptr() nsstsv_noexcept
    {
        return &m_value;
    }

private:
    value_type m_value;
};

} // namespace status_value_detail

#if nsstsv_CONFIG_NO_EXCEPTIONS

// Note: std::terminate() requires header <exception>.

template< typename S >
nsstsv_noreturn inline void report_bad_status_value_access( S && /*status*/ ) nsstsv_noexcept
{
    std::abort();
}

#else // nsstsv_CONFIG_NO_EXCEPTIONS

// Exception type to throw on unengaged access:

template< typename S >
class bad_status_value_access : public std::logic_error
{
public:
  // constructors
  bad_status_value_access() = delete;

  bad_status_value_access( S s )
  : std::logic_error( "status_value: bad status_value access" )
  , m_status( std::move( s ) )
  {}

  // destructor
  // ~bad_status_value_access() nsstsv_override {}

  // status observers
  S const & status() const nsstsv_noexcept
  {
      return m_status;
  }

private:
    S m_status;
};

template< typename S >
nsstsv_noreturn inline void report_bad_status_value_access( S && status )
{
    throw bad_status_value_access<typename std::remove_reference<S>::type>( std::forward<S>( status ) );
}

#endif // nsstsv_CONFIG_NO_EXCEPTIONS

// Status and optional value:
//
// Status shall be an object type and shall be copy constructible and destructible.
// Value shall be an object type and shall be move constructible and destructible.

template< typename S, typename V >
class status_value
{
public:
    typedef S status_type;
    typedef V value_type;

    // ?.?.3.1 constructors

    status_value() = delete;

    status_value( status_type s )
    : m_status( std::move( s ) )
    , m_has_value( false )
    {}

    status_value( status_type s, value_type const & v )
    : m_status( std::move( s ) )
    , m_has_value( true )
    {
        contained.construct_value( v );
    }

    status_value( status_type s, value_type && v )
    : m_status( std::move( s ) )
    , m_has_value( true )
    {
        contained.construct_value( std::move( v ) );
    }

    status_value( status_value && other )
    : m_status   ( std::move( other.m_status ) )
    , m_has_value( other.m_has_value )
    {
        if ( other.m_has_value )
        {
            contained.construct_value( std::move( other.contained.value() ) );
            other.contained.destruct_value();
            other.m_has_value = false;
        }
    }

    // ?.?.3.2 destructor

    ~status_value()
    {
        if ( m_has_value )
        {
            contained.destruct_value();
        }
    }

    // assignment

    status_value & operator=( status_value const & ) = delete;

    // ?.?.3.3 status observers

    status_type const & status() const nsstsv_noexcept
    {
        return m_status;
    }

    // ?.?.3.4 state observers

    constexpr bool has_value() const nsstsv_noexcept
    {
        return m_has_value;
    }

    constexpr explicit operator bool() const nsstsv_noexcept
    {
        return has_value();
    }

    // ?.?.3.5 value observers

    value_type const & value() const &
    {
        if ( ! has_value() )
            report_bad_status_value_access( m_status );

        return contained.value();
    }

    value_type & value() &
    {
        if ( ! has_value() )
            report_bad_status_value_access( m_status );

        return contained.value();
    }

    value_type && value() &&
    {
        if ( ! has_value() )
            report_bad_status_value_access( std::move( m_status ) );

        return std::move( contained ).value();
    }

    value_type const && value() const &&
    {
        if ( ! has_value() )
            report_bad_status_value_access( std::move( m_status ) );

        return std::move( contained ).value();
    }

    value_type const * operator->() const
    {
        if ( ! has_value() )
            report_bad_status_value_access( m_status );

        return contained.value_ptr();
    }

    value_type * operator->()
    {
        if ( ! has_value() )
            report_bad_status_value_access( m_status );

        return contained.value_ptr();
    }

    value_type const & operator *() const &
    {
        return value();
    }

    value_type & operator *() &
    {
        return value();
    }

    value_type const && operator*() const &&
    {
        return std::move( value() );
    }

    value_type && operator*() &&
    {
        return std::move( value() );
    }

private:
    using storage_type = status_value_detail::storage_t<status_type, value_type>;

    storage_type contained;
    status_type m_status;
    bool m_has_value;
};

} // namespace nonstd

#endif // NONSTD_STATUS_VALUE_HPP
