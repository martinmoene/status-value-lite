// Copyright 2016-2018 by Martin Moene
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
#define status_value_MINOR  0
#define status_value_PATCH  0

#define status_value_VERSION  nssv_STRINGIFY(status_value_MAJOR) "." nssv_STRINGIFY(status_value_MINOR) "." nssv_STRINGIFY(status_value_PATCH)

#define nssv_STRINGIFY(  x )  nssv_STRINGIFY_( x )
#define nssv_STRINGIFY_( x )  #x

// C++ language version detection (C++20 is speculative):
// Note: VC14.0/1900 (VS2015) lacks too much from C++14.

#if defined _MSVC_LANG
# define nssv_CPLUSPLUS  (_MSC_VER == 1900 ? 201103L : _MSVC_LANG )
#else
# define nssv_CPLUSPLUS  __cplusplus
#endif

#define nssv_CPP98_OR_GREATER  ( nssv_CPLUSPLUS >= 199711L )
#define nssv_CPP11_OR_GREATER  ( nssv_CPLUSPLUS >= 201103L )
#define nssv_CPP14_OR_GREATER  ( nssv_CPLUSPLUS >= 201402L )
#define nssv_CPP17_OR_GREATER  ( nssv_CPLUSPLUS >= 201703L )
#define nssv_CPP20_OR_GREATER  ( nssv_CPLUSPLUS >= 202000L )

#define nssv_CPP11_140  (nssv_CPP11_OR_GREATER || _MSC_VER >= 1900)
#define nssv_CPP14_000  (nssv_CPP14_OR_GREATER)
#define nssv_CPP17_000  (nssv_CPP17_OR_GREATER)

// Control presence of exception handling (try and auto discover):

#ifndef nssv_CONFIG_NO_EXCEPTIONS
# if defined(__cpp_exceptions) || defined(__EXCEPTIONS) || defined(_CPPUNWIND)
#  define nssv_CONFIG_NO_EXCEPTIONS  0
# else
#  define nssv_CONFIG_NO_EXCEPTIONS  1
# endif
#endif

// Presence of C++ language features:

#define nssv_HAVE_CONSTEXPR_14   nssv_CPP14_000
#define nssv_HAVE_NOEXCEPT       nssv_CPP11_140
#define nssv_HAVE_NORETURN       nssv_CPP17_000

#if nssv_HAVE_CONSTEXPR_14
# define nssv_constexpr14 constexpr
#else
# define nssv_constexpr14 /*constexpr*/
#endif

#if nssv_HAVE_NOEXCEPT && ! nssv_CONFIG_NO_EXCEPTIONS
# define nssv_noexcept noexcept
#else
# define nssv_noexcept /*noexcept*/
#endif

#if nssv_HAVE_NORETURN
# define nssv_noreturn [[noreturn]]
#else
# define nssv_noreturn /*[[noreturn]]*/
#endif

// Additional includes:

#if ! nssv_CONFIG_NO_EXCEPTIONS
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
    storage_t() nssv_noexcept {}
    ~storage_t() {}

    void construct_value( value_type const & v )
    {
        new( &m_value ) value_type( v );
    }

    void construct_value( value_type && v )
    {
        new( &m_value ) value_type( std::forward<V>( v ) );
    }

    void destruct_value() nssv_noexcept
    {
        m_value.~value_type();
    }

    constexpr value_type const & value() const & nssv_noexcept
    {
        return m_value;
    }

    value_type & value() & nssv_noexcept
    {
        return m_value;
    }

    constexpr value_type && value() const &&
    {
        return std::move( m_value );
    }

    value_type * value_ptr() const  nssv_noexcept
    {
        return &m_value;
    }

    value_type * value_ptr() nssv_noexcept
    {
        return &m_value;
    }

private:
    value_type m_value;
};

} // namespace status_value_detail

#if nssv_CONFIG_NO_EXCEPTIONS

template< typename S >
nssv_noreturn inline void report_bad_status_value_access( S & /*status*/ ) nssv_noexcept
{
    std::terminate();
}

#else

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
  ~bad_status_value_access() {}

  // status observers
  S const & status() const nssv_noexcept
  {
      return m_status;
  }

private:
    S m_status;
};

template< typename S >
inline void report_bad_status_value_access( S && status )
{
    throw bad_status_value_access<typename std::remove_reference<S>::type>( std::forward<S>( status ) );
}

#endif

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

    status_value(  status_type s, value_type const & v )
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

    status_type const & status() const nssv_noexcept
    {
        return m_status;
    }

    // ?.?.3.4 state observers

    constexpr bool has_value() const nssv_noexcept
    {
        return m_has_value;
    }

    constexpr explicit operator bool() const nssv_noexcept
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

        return std::move( contained.value() );
    }

    value_type const && value() const &&
    {
        if ( ! has_value() )
            report_bad_status_value_access( std::move( m_status ) );

        return std::move( contained.value() );
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
