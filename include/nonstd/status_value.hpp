// Copyright (C) 2016 Martin Moene.
//
// This version targets C++11 and later.
//
// This code is licensed under the MIT License (MIT).
//
// status_value is based on:
//   A Class for Status and Optional Value, P0262r0
//   by Lawrence Crowl and Chris Mysen

#ifndef NONSTD_STATUS_VALUE_HPP
#define NONSTD_STATUS_VALUE_HPP

#include <utility>

#define  status_value_VERSION "0.0.0"

// Compiler detection:

#define nsel_CPP11_OR_GREATER  ( __cplusplus >= 201103L )
#define nsel_CPP14_OR_GREATER  ( __cplusplus >= 201402L )

#if nsel_CPP14_OR_GREATER
# define nssv_constexpr14 constexpr
#else
# define nssv_constexpr14 /*constexpr*/
#endif

namespace nonstd {

template< typename S, typename V >
class status_value;

namespace status_value_detail {

/// union to hold value.

template< typename S, typename V >
union storage_t
{
    friend class status_value<S,V>;

private:
    typedef V value_type;

    // no-op construction
    storage_t() {}
    ~storage_t() {}

    void construct_value( value_type const & v )
    {
        new( &m_value ) value_type( v );
    }

    void construct_value( value_type && v )
    {
        new( &m_value ) value_type( std::forward<V>( v ) );
    }

    void destruct_value()
    {
        m_value.~value_type();
    }

    constexpr value_type const & value() const &
    {
        return m_value;
    }

    value_type & value() &
    {
        return m_value;
    }

    constexpr value_type && value() &&
    {
        return std::move( m_value );
    }

    value_type * value_ptr() const
    {
        return &m_value;
    }

    value_type * value_ptr()
    {
        return &m_value;
    }

private:
    value_type m_value;
};

} // namespace expected_detail

template< typename S, typename V >
class status_value
{
public:
    typedef S status_type;
    typedef V value_type;
    
    // Construction of status_value must include a status.

    status_value() = delete;

    // Construction of a status_value can be done with or without a value.

    status_value( status_type const & s )
    : m_status( s ) 
    , m_has_value( false ) 
    {}
    
    status_value( status_type const & s, value_type && v )
    : m_status( s ) 
    , m_has_value( true ) 
    {
        contained.construct_value( std::move( v ) );
    }
            
    status_value(  status_type const & s, value_type const & v )
    : m_status( s ) 
    , m_has_value( true ) 
    {
        contained.construct_value( v );
    }

    ~status_value()
    {
        if ( m_has_value )
        {
            contained.destruct_value();
        }
    }

    // A status_value may be moved. 
    // A copy operation would make the type unusable for non-copyable 
    // contained objects, so we do not provide a copy operation.

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

    // They may be queried for status. The design assumes that inlining 
    // will remove the cost of returning a reference for cheap copyable types.

    status_type const & status() const
    {
        return m_status;
    }

    // They may be queried for whether or not they have a value.

    bool has_value() const
    {
        return m_has_value;
    }
    
    operator bool() const
    {
        return has_value();
    }

    // They may provide access to their value. 
    // If they have no value, an exception of type Status, 
    // the status value passed to the constructor, is thrown.

    value_type const & value() const
    {
        if ( m_has_value )
            return contained.value();
            
        throw status_type( m_status );
    }
    
    value_type & value()
    {
        if ( m_has_value )
            return contained.value();
            
        throw status_type( m_status );
    }
    
    value_type const & operator *() const
    {
        return value();
    }
    
    value_type & operator *()
    {
        return value();
    }

    // This design enables moving out of the class by 
    // calling std::move on the result of the non-const functions. 

private:
    using storage_type = status_value_detail::storage_t<status_type, value_type >;
    
    storage_type contained;
    status_type m_status;
    bool m_has_value;
};

} // namespace nonstd

#endif // NONSTD_STATUS_VALUE_HPP
