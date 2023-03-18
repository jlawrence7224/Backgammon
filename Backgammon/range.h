//********* Copyright © Parametric Technology Corporation. All rights reserved
//*
//*
//*  Version : $Header: /MathcadClient/Trunk/env/inc/ms/range.h   10   2006-12-14 15:16:19-05:00   jbell $
//*
//*
//*  Purpose : Represents a range of values that reside in memory allocated by
//*            some other object.
//*
//*  Overview: Class Rng<value> represents an ordered sequence of objects of
//*            type 'value' that reside in a C++ vector. It implements a small
//*            but useful subset of the standard container semantics, allowing
//*            instances to be substituted for containers in certain contexts.
//*            It also provides a flexible and convenient way to pass around a
//*            fixed length sequence of values without actually copying them.
//*
//*            The underlying storage for the sequence is assumed to belong to
//*            some other object, however, so only functions that preserve the
//*            length of the sequence are available here.
//*
//*  Members : The default constructor produces  an empty range that returns 0
//*            from the function size().  A range can also be constructed from
//*            a pair of pointers into an array,  the first being aimed at the
//*            beginning of the range, and the second pointing one item beyond
//*            the final element of the range. A range can also be constructed
//*            from a pointer to the start of an array,  along with the number
//*            of items to be included in the range.
//*
//*            It's generally more convenient, however, to construct the range
//*            by calling the overloaded template function range().This method
//*            has the advantage that it is normally possible to automatically
//*            infer the template parameter from the surrounding call context.
//*
//*            take(r,i) returns a subrange consisting of the first i elements
//*            of r.
//*
//*            drop(r,i,j) omits the first i and final j elements of the range
//*            r to return the subrange [r.begin()+i,r.end()-j).
//*
//*            subrange(r,i,j) returns the subrange [r.begin()+i,r.begin()+j).
//*
//*            msrange(T,n) constructs a range of 'n' objects of type 'T', for
//*            which the underlying storage is allocated on the program stack.
//*            It is implemented as a macro, the expansion of which references
//*            'n' more than once,  so be careful that the expression 'n' does
//*            not include any kind of side effect.
//*
//*            All other members  have identical semantics to members of class
//*            std::vector<value> with the same name and are described in full
//*            elsewhere.
//*
//*  Comments: This file uses a tab size of 3 spaces.
//*
//*
//****************************************************************************

#ifndef  no_range
#define  no_range

#include <iterator>
#include <stdexcept>

using index = int;

template<class v>
class Rng
{
 public:                   // Synonyms
   typedef Rng              inherited;
   typedef size_t             size_type;
   typedef v                  value_type;
   typedef v&                 reference;
   typedef v*                 iterator;
   typedef v const*           const_iterator;
   typedef v const&           const_reference;
   typedef ptrdiff_t          difference_type;
   typedef v*                 reverse_iterator;
   typedef v const*           const_reverse_iterator;
   //typedef msrpointer(v)      reverse_iterator;
   //typedef mscrpointer(v)     const_reverse_iterator;

 public:                   // Construction
                              Rng(void);
                              Rng(reference);
                              Rng(iterator,iterator);
                              Rng(size_type,iterator);

 public:                   // Operations
            size_type         size(void)                          const;
            size_type         max_size(void)                      const;
            bool              empty(void)                         const;
            iterator          begin(void)                         const;
            iterator          end(void)                           const;
            reverse_iterator  rbegin(void)                        const;
            reverse_iterator  rend(void)                          const;
            reference         front(void)                         const;
            reference         back(void)                          const;
            reference         at(size_type)                       const;
            reference         operator[](size_type)               const;
            void              swap(Rng<v>&);

 public:                   // Coercions
                              operator const Rng<const v>&(void)const;
                              operator       Rng<const v>&(void);

 public:                   // Operators
 //friend   bool              operator==(Rng<v>,Rng<v>);
 //friend   bool              operator!=(Rng<v>,Rng<v>);
 //friend   bool              operator< (Rng<v>,Rng<v>);
 //friend   bool              operator> (Rng<v>,Rng<v>);
 //friend   bool              operator<=(Rng<v>,Rng<v>);
 //friend   bool              operator>=(Rng<v>,Rng<v>);

 private:                  // Representation
            iterator          m_beg;                     // The range offset
            iterator          m_end;                     // The range extent
};

//****************************************************************************

template<class v>Rng<v>     range(void);
template<class v>Rng<v>     range(v&);
template<class v>Rng<v>     range(v*,v*);
template<class v>Rng<v>     range(size_t,v*);
template<class v>Rng<v>     take(Rng<v>,index);
template<class v>Rng<v>     drop(Rng<v>,index,index = 0);
template<class v>Rng<v>     subrange(Rng<v>,index,index);

//****************************************************************************


                  /***   P U B L I C    M E M B E R S   ***/


//* RANGE (void)
//*
//* Construct an empty range of elements. As long as we supply the 'm_beg' and
//* 'm_end' iterators the same value we can use any address we like to specify
//* an empty range. An obvious choice of 0, however, tends to cause assertions
//* in the calling code to fail, since a null pointer is usually assumed to be
//* invalid.

template<class v>
Rng<v>::Rng(void)
    : m_beg(reinterpret_cast<iterator>(~0)),
    m_end(reinterpret_cast<iterator>(~0))
{}


//* RANGE (reference)
//*
//* Construct a range consisting of the single value 'rv'. We save the address
//* of 'rv' as the start of the range, then compute the end by adding 1 to it.

template<class v>
Rng<v>::Rng(reference rv)
    : m_beg(&rv),
    m_end(&rv + 1)
{}


//* RANGE (iterator,iterator)
//*
//* Construct a range from the array of values at address 'pb', up to, but not
//* including, the address 'pe'. These two pointers are presumed to point into
//* the same controlled sequence and may therefore be compared with each other.

template<class v>
Rng<v>::Rng(iterator pb, iterator pe)
    : m_beg(pb),
    m_end(pe)
{
    Assert((pb == 0) == (pe == 0));                           // Validate pointers
    Assert(pb <= pe);                                     // Validate range
}


//* RANGE (size_type,iterator)
//*
//* Construct a range from the 'sz' items found at address 'pb'. We save 'pb'
//* as the start of the range, then compute the end by adding 'sz' to it.

template<class v>
Rng<v>::Rng(size_type sz, iterator pb)
    : m_beg(pb),
    m_end(pb + sz)
{
    Assert((sz == 0) || (pb != 0));                           // Validate arguments
}


//* SIZE (void)
//*
//* Return the number of elements in the range. This is the difference between
//* the starting and ending addresses of the underlying array.

template<class v>
typename Rng<v>::size_type Rng<v>::size(void) const
{
    return m_end - m_beg;                                 // Elements in range
}


//* MAX SIZE (void)
//*
//* Return the maximum number of values that can be stored in the range. Since
//* the underlying storage for the sequence is assumed to belong to some other
//* object, we can neither grow nor shrink this array, and the maximum size is
//* therefore locked to the current size of the range.

template<class v>
typename Rng<v>::size_type Rng<v>::max_size(void) const
{
    return size();                                        // Delegate to size()
}


//* EMPTY (void)
//*
//* Return true if the range is empty. This is the case precisely when the two
//* iterators carry the same value, even if this does happen to be the special
//* constant '~0' that was supplied by the default constructor.

template<class v>
bool Rng<v>::empty(void) const
{
    return m_end == m_beg;                                // Is range empty?
}


//* BEGIN (void)
//*
//* Return an iterator pointing to the start of the range. This is precisely
//* the meaning of the 'm_beg' data member.

template<class v>
typename Rng<v>::iterator Rng<v>::begin(void) const
{
    return m_beg;                                         // Start of range
}


//* END (void)
//*
//* Return an iterator pointing to the end of the range. This is precisely
//* the meaning of the 'm_end' data member.

template<class v>
typename Rng<v>::iterator Rng<v>::end(void) const
{
    return m_end;                                         // End of range
}


//* R BEGIN (void)
//*
//* Return a reverse iterator that points to the end of the range, and that
//* 'advances' backwards towards the start with each call to operator++().

template<class v>
typename Rng<v>::reverse_iterator Rng<v>::rbegin(void) const
{
    return reverse_iterator(m_end);                       // Reverse iterator
}


//* R END (void)
//*
//* Return a reverse iterator that points to the start of the range, and that
//* marks the end of a reverse iteration over the elements.

template<class v>
typename Rng<v>::reverse_iterator Rng<v>::rend(void) const
{
    return reverse_iterator(m_beg);                       // Reverse iterator
}


//* FRONT (void)
//*
//* Return a reference to the first value included in the range. Note that it
//* is a logical error to invoke this member for an empty range.

template<class v>
typename Rng<v>::reference Rng<v>::front(void) const
{
    Assert(!empty());                                     // Validate argument

    return m_beg[0];                                      // Return first item
}


//* BACK (void)
//*
//* Return a reference to the last value included in the range. Note that it
//* is a logical error to invoke this member for an empty range.

template<class v>
typename Rng<v>::reference Rng<v>::back(void) const
{
    Assert(!empty());                                     // Validate argument

    return m_end[-1];                                     // Return last item
}


//* AT (size_type)
//*
//* Return a reference to the 'ii'th value included in the range, or throw an
//* exception if the index 'ii' is out of range.

template<class v>
typename Rng<v>::reference Rng<v>::at(size_type ii) const
{
    if (size() <= ii)                                     // Index out of range?
    {
        Trace("ms: index %u out of range (%u).", ii, size());// ...helpful warning

        throw std::out_of_range("ms: index out of range"); // ...throw exception
    }

    return m_beg[ii];                                     // Return 'ii'th item
}


//* OPERATOR []
//*
//* Return a reference to the 'ii'th element of the range. This function skips
//* the range check performed by the function at() in release builds.

template<class v>
typename Rng<v>::reference Rng<v>::operator[](size_type ii) const
{
    Assert(ii < size());                                  // Validate argument

    return m_beg[ii];                                     // Return 'ii'th item
}


//* SWAP (Rng<v>&)
//*
//* Swap the iterators held by this object and the range 'rr'.

template<class v>
void Rng<v>::swap(Rng<v>& rr)
{
    swap(m_beg, rr.m_beg);                                 // Swap 'm_beg'
    swap(m_end, rr.m_end);                                 // Swap 'm_end'
}


//* OPERATOR const Rng<const v>& (void)
//*
//* Return a reference to this object formatted as a range of constant values.
//* The existence of this coercion operator enables the compiler to perform an
//* implicit coercion from type Rng<v> to type Rng<const v>, and is always
//* a safe thing to do.

template<class v>
Rng<v>::operator const Rng<const v>& (void) const
{
    return *reinterpret_cast<const Rng<const v>*>(this);            // Force coercion
}


//* OPERATOR Rng<const v>& (void)
//*
//* Return a reference to this object formatted as a range of constant values.
//* The existence of this coercion operator enables the compiler to perform an
//* implicit coercion from type Rng<v> to type Rng<const v>, and is always
//* a safe thing to do.

template<class v>
Rng<v>::operator Rng<const v>& (void)
{
    return *reinterpret_cast<Rng<const v>*>(this);                 // Force coercion
}


/***   F R I E N D    F U N C T I O N S   ***/


//* OPERATOR == (Rng<v>,Rng<v>)
//*
//* Return true if the two ranges 'ra' and 'rb' decribe identical sequences of
//* elements. This is the case if and only if both ranges have the same length
//* and each pair of elements compare equal to one another.

template<class v>
bool operator==(Rng<v> lr, Rng<v> rr)
{
    return lr.size() == rr.size() && std::equal(lr.begin(), lr.end(), rr.begin());
}


//* OPERATOR != (Rng<v>,Rng<v>)
//*
//* Return true if the two ranges 'ra' and 'rb' describe distinct sequences of
//* elements. Implemented in the usual way by delegating to operator==().

template<class v>
bool operator!=(Rng<v> ra, Rng<v> rb)
{
    return !(ra == rb);                                   // Delegate to ==
}


//* OPERATOR < (Rng<v>,Rng<v>)
//*
//* Return true if the range 'ra' is 'less than' the range 'rb', where 'less
//* than' has the same meaning as it does for the standard vector class.

template<class v>
bool operator<(Rng<v> ra, Rng<v> rb)
{
    return std::lexicographical_compare(ra.begin(), ra.end(), rb.begin(), rb.end());
}


//* OPERATOR > (Rng<v>,Rng<v>)
//*
//* Implemented in the usual way by delegating to operator<().

template<class v>
bool operator> (Rng<v> lr, Rng<v> rr)
{
    return rr < lr;                                       // Delegate to <
}


//* OPERATOR <= (Rng<v>,Rng<v>)
//*
//* Implemented in the usual way by delegating to operator<().

template<class v>
bool operator<=(Rng<v> lr, Rng<v> rr)
{
    return !(rr < lr);                                    // Delegate to <
}


//* OPERATOR >= (Rng<v>,Rng<v>)
//*
//* Implemented in the usual way by delegating to operator<().

template<class v>
bool operator>=(Rng<v> lr, Rng<v> rr)
{
    return !(lr < rr);                                    // Delegate to <
}


/***   F R E E    F U N C T I O N S   ***/


//* RANGE (void)
//*
//* Construct and return an empty range.

template<class v>
Rng<v> range(void)
{
    return Rng<v>();                                    // Construct range
}


//* RANGE (v&)
//*
//* Construct and return a range consisting of the single value 'rv'.

template<class v>
Rng<v> range(v& rv)
{
    return Rng<v>(rv);                                  // Construct range
}


//* RANGE (v*,v*)
//*
//* Construct and return a range from the values found at address 'pb', up to,
//* but not including the address 'pe'.

template<class v>
Rng<v> range(v* pb, v* pe)
{
    return Rng<v>(pb, pe);                               // Construct range
}


//* RANGE (size_t,v*)
//*
//* Construct and return a range from the 'sz' items found at address 'pb'. We
//* use 'pb' as the start of the range, then compute the end by adding 'sz' to
//* it.

template<class v>
Rng<v> range(size_t sz, v* pb)
{
    return Rng<v>(pb, pb + sz);                            // Construct range
}


//* TAKE (Rng<v>,index)
//*
//* Take the first 'ii' elements of 'rv' to yield the subrange [rv.begin(),rv.
//* begin()+ii).

template<class v>
Rng<v> take(Rng<v> rv, index ii)
{
    Assert(ii <= rv.size());                              // Validate argument

    return Rng<v>(rv.begin(), rv.begin() + ii);            // Construct subrange
}


//* DROP (Rng<v>,index,index)
//*
//* Drop the first 'ii' and final 'jj' elements of the range 'rv' to yield the
//* subrange [rv.begin()+ii,rv.end()-jj).

template<class v>
Rng<v> drop(Rng<v> rv, index ii, index jj)
{
    Assert(ii + jj <= rv.size());                           // Validate arguments

    return Rng<v>(rv.begin() + ii, rv.end() - jj);           // Construct subrange
}


//* SUB RANGE (Rng<v>,index,index)
//*
//* Return the subrange [rv.begin()+ii,rv.end()+jj).

template<class v>
Rng<v> subrange(Rng<v> rv, index ii, index jj)
{
    Assert(ii <= jj && jj <= rv.size());                    // Validate arguments

    return Rng<v>(rv.begin() + ii, rv.begin() + jj);         // Construct subrange
}


//* VALID (Rng<v>)
//*
//* Return true if the range 'rv' holds valid iterators; that is, values other
//* than those special values assigned by the default constructor. Notice that
//* it suffices to test only the begin() iterator, since either both are valid
//* or else neither are.

template<class v>
bool valid(Rng<v> rv)
{
    Assert(rv.begin() != reinterpret_cast<Rng<v>::template iterator>(~0) || rv.empty());

    return (rv.begin() != reinterpret_cast<Rng<v>::template iterator>(~0));	// Is iterator valid?
}


/***   P U B L I C    M A C R O S   ***/


//* MS RANGE (type,count)
//*
//* Allocate on the stack a range of 'count' objects of the specified type. Be
//* aware that the 'count' parameter is evaluated twice in the macro expansion
//* and so should almost certainly not include any kind of side effect.

#define msrange(type,count) \
   Rng<type>(count,msnew(type,count))                  // Allocate on stack

#endif
