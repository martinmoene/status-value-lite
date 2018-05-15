// Copyright (C) 2016 Martin Moene.
//
// This version targets C++98 and later.
//
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// status_value is based on:
//   A Class for Status and Optional Value, P0262r0
//   by Lawrence Crowl and Chris Mysen

#ifndef NONSTD_STATUS_VALUE_HPP
#define NONSTD_STATUS_VALUE_HPP

#include <cassert>
#include <stdexcept>
#include <utility>

#define  status_value_VERSION "0.0.0"

// Configuration:

#ifndef  nssv_FEATURE_MAX_ALIGN_HACK
# define nssv_FEATURE_MAX_ALIGN_HACK  0
#endif

#ifndef nssv_FEATURE_ALIGN_AS
// used in #if defined(), so no default...
#endif

#ifndef  nssv_FEATURE_ALIGN_AS_FALLBACK
# define nssv_FEATURE_ALIGN_AS_FALLBACK  double
#endif

// Compiler detection:

#if defined(_MSC_VER)
# define nssv_COMPILER_MSVC_VERSION   (_MSC_VER / 100 - 5 - (_MSC_VER < 1900))
#else
# define nssv_COMPILER_MSVC_VERSION   0
#endif

#define nssv_COMPILER_IS_VC6   ( nssv_COMPILER_MSVC_VERSION == 6 )

#define nssv_CPP11_OR_GREATER  ( __cplusplus >= 201103L )
#define nssv_CPP14_OR_GREATER  ( __cplusplus >= 201402L )

// Presence of C++11 language features:

#if nssv_CPP11_OR_GREATER || nssv_COMPILER_MSVC_VERSION >= 14
# define nssv_HAVE_CONSTEXPR_11  1
#endif

// Presence of C++14 language features:

#if nssv_CPP14_OR_GREATER
# define nssv_HAVE_CONSTEXPR_14  1
#endif

// For the rest, consider VC12, VC14 as C++11 for GSL Lite:

#if nssv_COMPILER_MSVC_VERSION >= 12
# undef  nssv_CPP11_OR_GREATER
# define nssv_CPP11_OR_GREATER  1
#endif

// C++ feature usage:

#if nssv_HAVE_CONSTEXPR_11
# define nssv_constexpr constexpr
#else
# define nssv_constexpr /*nothing*/
#endif

#if nssv_HAVE_CONSTEXPR_14
# define nssv_constexpr14 constexpr
#else
# define nssv_constexpr14 /*nothing*/
#endif

/* Object allocation and alignment
 *
 * optional lite reserves POD-type storage for an object of the underlying type
 * inside a union to prevent unwanted construction and uses placement new to
 * construct the object when required. Using non-placement new (malloc) to
 * obtain storage, ensures that the memory is properly aligned for the object's
 * type, whereas that's not the case with placement new.
 *
 * If you access data that's not properly aligned, it 1) may take longer than
 * when it is properly aligned (on x86 processors), or 2) it may terminate
 * the program immediately (many other processors).
 *
 * Although the C++ standard does not guarantee that all user-defined types
 * have the alignment of some POD type, in practice it's likely they do.
 *
 * If optional lite is compiled as C++11 or later, C++11 alignment facilities
 * are used for storage of the underlying object. When compiling with C++03,
 * optional lite tries to determine proper alignment using meta programming.
 * If this doesn't work out, you can control alignment via three macros.
 *
 * optional lite uses the following rules for alignment:
 *
 * 1. If the program compiles as C++11 or later, C++11 alignment facilities
 * are used.
 *
 * 2. If you define -Dnssv_FEATURE_MAX_ALIGN_HACK=1 the underlying
 * type is aligned as the most restricted type in `struct max_align_t`. This
 * potentially wastes many bytes per optional if the actually required
 * alignment is much less, e.g. 24 bytes used instead of the 2 bytes required.
 *
 * 3. If you define -Dnssv_FEATURE_ALIGN_AS='pod-type' the
 * underlying type is aligned as 'pod-type'. It's your obligation to specify a
 * type with proper alignment.
 *
 * 4. If you define -Dnssv_FEATURE_ALIGN_AS_FALLBACK='pod-type' the
 * fallback type for alignment of rule 5 below becomes 'pod-type'. It's your
 * obligation to specify a type with proper alignment.
 *
 * 5. At default, optional lite tries to find a POD type with the same
 * alignment as the underlying type.
 *
 * The algorithm for alignment of 5. is:
 * - Determine the alignment A of the underlying type using `alignment_of<>`.
 * - Find a POD type from the list `alignment_types` with exactly alignment A.
 * - If no such POD type is found, use a type with a relatively strict
 *   alignment requirement such as double; this type is specified in
 *   `nssv_FEATURE_ALIGN_AS_FALLBACK` (default double).
 *
 * Note that the algorithm of 5. differs from the one Andrei Alexandrescu uses
 * in Generic<Programming>: Discriminated Unions, part 2 (see below).
 *
 * The class template `alignment_of<>` is gleaned from Boost.TypeTraits,
 * The storage type `storage_t<>` is adapted from the one I created for
 * spike-expected.
 *
 * See also:
 *
 * Andrei Alexandrescu. Generic<Programming>: Discriminated Unions (I-III). CUJ. April 2002.
 * - http://collaboration.cmc.ec.gc.ca/science/rpn/biblio/ddj/Website/articles/CUJ/2002/cexp2004/alexandr/alexandr.htm
 * - http://collaboration.cmc.ec.gc.ca/science/rpn/biblio/ddj/Website/articles/CUJ/2002/cexp2006/alexandr/alexandr.htm
 * - http://collaboration.cmc.ec.gc.ca/science/rpn/biblio/ddj/Website/articles/CUJ/2002/cexp2008/alexandr/alexandr.htm
 */

namespace nonstd {

template< typename S, typename V >
class status_value;

namespace status_value_detail {

#if nssv_FEATURE_MAX_ALIGN_HACK

// Max align, use most restricted type for alignment:

#define nssv_UNIQUE(  name )       nssv_UNIQUE2( name, __LINE__ )
#define nssv_UNIQUE2( name, line ) nssv_UNIQUE3( name, line )
#define nssv_UNIQUE3( name, line ) name ## line

#define nssv_ALIGN_TYPE( type ) \
    type nssv_UNIQUE( _t ); struct_t< type > nssv_UNIQUE( _st )

template< typename T >
struct struct_t { T _; };

union max_align_t
{
    nssv_ALIGN_TYPE( char );
    nssv_ALIGN_TYPE( short int );
    nssv_ALIGN_TYPE( int );
    nssv_ALIGN_TYPE( long int  );
    nssv_ALIGN_TYPE( float  );
    nssv_ALIGN_TYPE( double );
    nssv_ALIGN_TYPE( long double );
    nssv_ALIGN_TYPE( char * );
    nssv_ALIGN_TYPE( short int * );
    nssv_ALIGN_TYPE( int *  );
    nssv_ALIGN_TYPE( long int * );
    nssv_ALIGN_TYPE( float * );
    nssv_ALIGN_TYPE( double * );
    nssv_ALIGN_TYPE( long double * );
    nssv_ALIGN_TYPE( void * );

#ifdef HAVE_LONG_LONG
    nssv_ALIGN_TYPE( long long );
#endif

    struct Unknown;

    Unknown ( * nssv_UNIQUE(_) )( Unknown );
    Unknown * Unknown::* nssv_UNIQUE(_);
    Unknown ( Unknown::* nssv_UNIQUE(_) )( Unknown );

    struct_t< Unknown ( * )( Unknown)         > nssv_UNIQUE(_);
    struct_t< Unknown * Unknown::*            > nssv_UNIQUE(_);
    struct_t< Unknown ( Unknown::* )(Unknown) > nssv_UNIQUE(_);
};

#undef nssv_UNIQUE
#undef nssv_UNIQUE2
#undef nssv_UNIQUE3

#undef nssv_ALIGN_TYPE

#elif defined( nssv_FEATURE_ALIGN_AS ) // nssv_FEATURE_MAX_ALIGN_HACK

// Use user-specified type for alignment:

#define nssv_ALIGN_AS( unused ) \
    nssv_FEATURE_ALIGN_AS

#else // nssv_FEATURE_MAX_ALIGN_HACK

// Determine POD type to use for alignment:

#define nssv_ALIGN_AS( to_align ) \
    typename type_of_size< alignment_types, alignment_of< to_align >::value >::type

#if nssv_COMPILER_IS_VC6

template< bool condition, typename Then, typename Else >
struct select
{
  template < bool > struct selector;

  template <> struct selector< true  > { typedef Then type; };
  template <> struct selector< false > { typedef Else type; };

  typedef typename selector< condition >::type type;
};

#else // nssv_COMPILER_IS_VC6

template < bool condition, typename Then, typename Else > struct select;
template < typename Then, typename Else > struct select< true , Then, Else > { typedef Then type; };
template < typename Then, typename Else > struct select< false, Then, Else > { typedef Else type; };

#endif // nssv_COMPILER_IS_VC6

template <typename T>
struct alignment_of;

template <typename T>
struct alignment_of_hack
{
    char c;
    T t;
    alignment_of_hack();
};

template <unsigned A, unsigned S>
struct alignment_logic
{
    enum { value = A < S ? A : S };
};

template< typename T >
struct alignment_of
{
    enum { value = alignment_logic<
        sizeof( alignment_of_hack<T> ) - sizeof(T), sizeof(T) >::value, };
};

struct nulltype{};

template< typename Head, typename Tail >
struct typelist
{
    typedef Head head;
    typedef Tail tail;
};

template< typename List, size_t N >
struct type_of_size
{
    typedef typename select<
        N == sizeof( typename List::head ),
            typename List::head,
            typename type_of_size<typename List::tail, N >::type >::type type;
};

#if ! nssv_COMPILER_IS_VC6

template< size_t N >
struct type_of_size< nulltype, N >
{
    typedef nssv_FEATURE_ALIGN_AS_FALLBACK type;
};

#else // nssv_COMPILER_IS_VC6

// VC6: no partial specialization

#define MK_TYPE_OF_SIZE( n ) \
    template<> \
    struct type_of_size< nulltype, n > \
    { \
        typedef nssv_FEATURE_ALIGN_AS_FALLBACK type; \
    }

MK_TYPE_OF_SIZE( 1  );
MK_TYPE_OF_SIZE( 2  );
MK_TYPE_OF_SIZE( 4  );
MK_TYPE_OF_SIZE( 8  );
MK_TYPE_OF_SIZE( 12 );
MK_TYPE_OF_SIZE( 16 );
MK_TYPE_OF_SIZE( 20 );
MK_TYPE_OF_SIZE( 24 );
MK_TYPE_OF_SIZE( 28 );
MK_TYPE_OF_SIZE( 32 );

#undef MK_TYPE_OF_SIZE

#endif // nssv_COMPILER_IS_VC6

template< typename T>
struct struct_t { T _; };

#define nssv_ALIGN_TYPE( type ) \
    typelist< type , typelist< struct_t< type >

struct Unknown;

typedef
    nssv_ALIGN_TYPE( char ),
    nssv_ALIGN_TYPE( short ),
    nssv_ALIGN_TYPE( int ),
    nssv_ALIGN_TYPE( long ),
    nssv_ALIGN_TYPE( float ),
    nssv_ALIGN_TYPE( double ),
    nssv_ALIGN_TYPE( long double ),

    nssv_ALIGN_TYPE( char *),
    nssv_ALIGN_TYPE( short * ),
    nssv_ALIGN_TYPE( int * ),
    nssv_ALIGN_TYPE( long * ),
    nssv_ALIGN_TYPE( float * ),
    nssv_ALIGN_TYPE( double * ),
    nssv_ALIGN_TYPE( long double * ),

    nssv_ALIGN_TYPE( Unknown ( * )( Unknown ) ),
    nssv_ALIGN_TYPE( Unknown * Unknown::*     ),
    nssv_ALIGN_TYPE( Unknown ( Unknown::* )( Unknown ) ),

    nulltype
    > > > > > > >    > > > > > > >
    > > > > > > >    > > > > > > >
    > > > > > >
    alignment_types;

#undef nssv_ALIGN_TYPE

#endif // nssv_FEATURE_MAX_ALIGN_HACK

/// C++98 union to hold value.

template< typename S, typename V >
union storage_t
{
    friend class status_value<S,V>;

private:
    typedef V value_type;

    // no-op construction
    storage_t() {}
    ~storage_t() {}

    storage_t( value_type const & v )
    {
        construct_value( v );
    }

    void construct_value( value_type const & v )
    {
        ::new( value_ptr() ) value_type( v );
    }

#if nssv_CPP11_OR_GREATER

    void construct_value( value_type && v )
    {
        new( value_ptr() ) value_type( std::forward<V>( v ) );
    }
#endif

    void destruct_value()
    {
        // Note: VC6 requires the use of the
        // template parameter T (cannot use value_type).
        value_ptr()->~V();
    }

#if nssv_CPP11_OR_GREATER

    constexpr value_type const & value() const &
    {
        return * value_ptr();
    }

    value_type & value() &
    {
        return * value_ptr();
    }

    constexpr value_type && value() &&
    {
        return std::move( * value_ptr() );
    }
#else

    value_type const & value() const
    {
        return * value_ptr();
    }

    value_type & value()
    {
        return * value_ptr();
    }
#endif

    value_type * value_ptr() const
    {
        return as( (value_type*)0 );
    }

    value_type * value_ptr()
    {
        return as( (value_type*)0 );
    }

#if nssv_CPP11_OR_GREATER

    using aligned_storage_t = typename std::aligned_storage< sizeof(value_type), alignof(value_type) >::type;
    aligned_storage_t buffer;

#elif nssv_FEATURE_MAX_ALIGN_HACK

    typedef struct { unsigned char data[ sizeof(value_type) ]; } aligned_storage_t;

    max_align_t hack;
    aligned_storage_t buffer;

#else
    typedef nssv_ALIGN_AS(value_type) align_as_type;

    typedef struct { align_as_type data[ 1 + ( sizeof(value_type) - 1 ) / sizeof(align_as_type) ]; } aligned_storage_t;
    aligned_storage_t buffer;

#   undef nssv_ALIGN_AS

#endif // nssv_FEATURE_MAX_ALIGN_HACK

    // Note: VC6 cannot handle as<T>():

    template <typename U>
    U * as( U* ) const
    {
        return reinterpret_cast<U*>( const_cast<aligned_storage_t *>( &buffer ) );
    }
};

} // namespace expected_detail

template< typename S, typename V >
class status_value
{
public:
    typedef S status_type;
    typedef V value_type;
    
    // Construction of status_value must include a status.

#if nssv_CPP11_OR_GREATER
    status_value() = delete;
#else
private:
    status_value();
public:
#endif

    // Construction of a status_value can be done with or without a value.

    status_value( status_type const & s )
    : m_status( s ) 
    , m_has_value( false ) 
    {}
    
#if nssv_CPP11_OR_GREATER
    status_value( status_type const & s, value_type && v )
    : m_status( s ) 
    , m_has_value( true ) 
    {
        contained.construct_value( std::move( v ) );
    }
#endif 
           
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

#if nssv_CPP11_OR_GREATER

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
#else

    status_value( status_value const & other )
    : m_status   ( other.m_status )
    , m_has_value( other.m_has_value ) 
    {
        if ( other.m_has_value ) 
        {
            contained.construct_value( other.contained.value() );
        }
    }
#endif

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
    typedef status_value_detail::storage_t<status_type, value_type > storage_type;
    
    storage_type contained;
    status_type m_status;
    bool m_has_value;
};

} // namespace nonstd

#endif // NONSTD_STATUS_VALUE_HPP
