//********* Copyright © Parametric Technology Corporation. All rights reserved
//*
//*
//*  Version : $Header: /MathcadClient/Trunk/env/inc/ms/range.i   11   2006-12-14 15:16:19-05:00   jbell $
//*
//*
//*  Purpose : Inline implementations for class Range.
//*
//*
//*  Comments: This file uses a tab size of 3 spaces.
//*
//*
//****************************************************************************

#ifndef  no_range
# error  This file should only be included by range.h.
#endif


                  /***   P U B L I C    M E M B E R S   ***/


//* RANGE (void)
//*
//* Construct an empty range of elements. As long as we supply the 'm_beg' and
//* 'm_end' iterators the same value we can use any address we like to specify
//* an empty range. An obvious choice of 0, however, tends to cause assertions
//* in the calling code to fail, since a null pointer is usually assumed to be
//* invalid.

template<class v>
   Range<v>::Range(void)
           : m_beg(rcast<iterator>(~0)),
             m_end(rcast<iterator>(~0))
{}


//* RANGE (reference)
//*
//* Construct a range consisting of the single value 'rv'. We save the address
//* of 'rv' as the start of the range, then compute the end by adding 1 to it.

template<class v>
   Range<v>::Range(reference rv)
           : m_beg(&rv),
             m_end(&rv+1)
{}


//* RANGE (iterator,iterator)
//*
//* Construct a range from the array of values at address 'pb', up to, but not
//* including, the address 'pe'. These two pointers are presumed to point into
//* the same controlled sequence and may therefore be compared with each other.

template<class v>
   Range<v>::Range(iterator pb,iterator pe)
           : m_beg(pb),
             m_end(pe)
{
   Assert((pb==0) == (pe==0));                           // Validate pointers
   Assert(pb <= pe);                                     // Validate range
}


//* RANGE (size_type,iterator)
//*
//* Construct a range from the 'sz' items found at address 'pb'. We save 'pb'
//* as the start of the range, then compute the end by adding 'sz' to it.

template<class v>
   Range<v>::Range(size_type sz,iterator pb)
           : m_beg(pb),
             m_end(pb+sz)
{
   Assert((sz==0) || (pb!=0));                           // Validate arguments
}


//* SIZE (void)
//*
//* Return the number of elements in the range. This is the difference between
//* the starting and ending addresses of the underlying array.

template<class v>
typename Range<v>::size_type Range<v>::size(void) const
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
typename Range<v>::size_type Range<v>::max_size(void) const
{
   return size();                                        // Delegate to size()
}


//* EMPTY (void)
//*
//* Return true if the range is empty. This is the case precisely when the two
//* iterators carry the same value, even if this does happen to be the special
//* constant '~0' that was supplied by the default constructor.

template<class v>
bool Range<v>::empty(void) const
{
   return m_end == m_beg;                                // Is range empty?
}


//* BEGIN (void)
//*
//* Return an iterator pointing to the start of the range. This is precisely
//* the meaning of the 'm_beg' data member.

template<class v>
typename Range<v>::iterator Range<v>::begin(void) const
{
   return m_beg;                                         // Start of range
}


//* END (void)
//*
//* Return an iterator pointing to the end of the range. This is precisely
//* the meaning of the 'm_end' data member.

template<class v>
typename Range<v>::iterator Range<v>::end(void) const
{
   return m_end;                                         // End of range
}


//* R BEGIN (void)
//*
//* Return a reverse iterator that points to the end of the range, and that
//* 'advances' backwards towards the start with each call to operator++().

template<class v>
typename Range<v>::reverse_iterator Range<v>::rbegin(void) const
{
   return reverse_iterator(m_end);                       // Reverse iterator
}


//* R END (void)
//*
//* Return a reverse iterator that points to the start of the range, and that
//* marks the end of a reverse iteration over the elements.

template<class v>
typename Range<v>::reverse_iterator Range<v>::rend(void) const
{
   return reverse_iterator(m_beg);                       // Reverse iterator
}


//* FRONT (void)
//*
//* Return a reference to the first value included in the range. Note that it
//* is a logical error to invoke this member for an empty range.

template<class v>
typename Range<v>::reference Range<v>::front(void) const
{
   Assert(!empty());                                     // Validate argument

   return m_beg[0];                                      // Return first item
}


//* BACK (void)
//*
//* Return a reference to the last value included in the range. Note that it
//* is a logical error to invoke this member for an empty range.

template<class v>
typename Range<v>::reference Range<v>::back(void) const
{
   Assert(!empty());                                     // Validate argument

   return m_end[-1];                                     // Return last item
}


//* AT (size_type)
//*
//* Return a reference to the 'ii'th value included in the range, or throw an
//* exception if the index 'ii' is out of range.

template<class v>
typename Range<v>::reference Range<v>::at(size_type ii) const
{
   if (size() <= ii)                                     // Index out of range?
   {
      Trace("ms: index %u out of range (%u).",ii,size());// ...helpful warning

      throw std::out_of_range("ms: index out of range"); // ...throw exception
   }

   return m_beg[ii];                                     // Return 'ii'th item
}


//* OPERATOR []
//*
//* Return a reference to the 'ii'th element of the range. This function skips
//* the range check performed by the function at() in release builds.

template<class v>
typename Range<v>::reference Range<v>::operator[](size_type ii) const
{
   Assert(ii < size());                                  // Validate argument

   return m_beg[ii];                                     // Return 'ii'th item
}


//* SWAP (Range<v>&)
//*
//* Swap the iterators held by this object and the range 'rr'.

template<class v>
void Range<v>::swap(Range<v>& rr)
{
   swap(m_beg,rr.m_beg);                                 // Swap 'm_beg'
   swap(m_end,rr.m_end);                                 // Swap 'm_end'
}


//* OPERATOR const Range<const v>& (void)
//*
//* Return a reference to this object formatted as a range of constant values.
//* The existence of this coercion operator enables the compiler to perform an
//* implicit coercion from type Range<v> to type Range<const v>, and is always
//* a safe thing to do.

template<class v>
Range<v>::operator const Range<const v>&(void) const
{
   return *rcast<const Range<const v>*>(this);            // Force coercion
}


//* OPERATOR Range<const v>& (void)
//*
//* Return a reference to this object formatted as a range of constant values.
//* The existence of this coercion operator enables the compiler to perform an
//* implicit coercion from type Range<v> to type Range<const v>, and is always
//* a safe thing to do.

template<class v>
Range<v>::operator Range<const v>&(void)
{
   return *rcast<Range<const v>*>(this);                 // Force coercion
}


                /***   F R I E N D    F U N C T I O N S   ***/


//* OPERATOR == (Range<v>,Range<v>)
//*
//* Return true if the two ranges 'ra' and 'rb' decribe identical sequences of
//* elements. This is the case if and only if both ranges have the same length
//* and each pair of elements compare equal to one another.

template<class v>
bool operator==(Range<v> lr,Range<v> rr)
{
   return lr.size()==rr.size() && std::equal(lr.begin(),lr.end(),rr.begin());
}


//* OPERATOR != (Range<v>,Range<v>)
//*
//* Return true if the two ranges 'ra' and 'rb' describe distinct sequences of
//* elements. Implemented in the usual way by delegating to operator==().

template<class v>
bool operator!=(Range<v> ra,Range<v> rb)
{
   return !(ra == rb);                                   // Delegate to ==
}


//* OPERATOR < (Range<v>,Range<v>)
//*
//* Return true if the range 'ra' is 'less than' the range 'rb', where 'less
//* than' has the same meaning as it does for the standard vector class.

template<class v>
bool operator<(Range<v> ra,Range<v> rb)
{
   return std::lexicographical_compare(ra.begin(),ra.end(),rb.begin(),rb.end());
}


//* OPERATOR > (Range<v>,Range<v>)
//*
//* Implemented in the usual way by delegating to operator<().

template<class v>
bool operator> (Range<v> lr,Range<v> rr)
{
   return rr < lr;                                       // Delegate to <
}


//* OPERATOR <= (Range<v>,Range<v>)
//*
//* Implemented in the usual way by delegating to operator<().

template<class v>
bool operator<=(Range<v> lr,Range<v> rr)
{
   return !(rr < lr);                                    // Delegate to <
}


//* OPERATOR >= (Range<v>,Range<v>)
//*
//* Implemented in the usual way by delegating to operator<().

template<class v>
bool operator>=(Range<v> lr,Range<v> rr)
{
   return !(lr < rr);                                    // Delegate to <
}


                  /***   F R E E    F U N C T I O N S   ***/


//* RANGE (void)
//*
//* Construct and return an empty range.

template<class v>
Range<v> range(void)
{
   return Range<v>();                                    // Construct range
}


//* RANGE (v&)
//*
//* Construct and return a range consisting of the single value 'rv'.

template<class v>
Range<v> range(v& rv)
{
   return Range<v>(rv);                                  // Construct range
}


//* RANGE (v*,v*)
//*
//* Construct and return a range from the values found at address 'pb', up to,
//* but not including the address 'pe'.

template<class v>
Range<v> range(v* pb,v* pe)
{
   return Range<v>(pb,pe);                               // Construct range
}


//* RANGE (size_t,v*)
//*
//* Construct and return a range from the 'sz' items found at address 'pb'. We
//* use 'pb' as the start of the range, then compute the end by adding 'sz' to
//* it.

template<class v>
Range<v> range(size_t sz,v* pb)
{
   return Range<v>(pb,pb+sz);                            // Construct range
}


//* TAKE (Range<v>,index)
//*
//* Take the first 'ii' elements of 'rv' to yield the subrange [rv.begin(),rv.
//* begin()+ii).

template<class v>
Range<v> take(Range<v> rv,index ii)
{
   Assert(ii <= rv.size());                              // Validate argument

   return Range<v>(rv.begin(),rv.begin()+ii);            // Construct subrange
}


//* DROP (Range<v>,index,index)
//*
//* Drop the first 'ii' and final 'jj' elements of the range 'rv' to yield the
//* subrange [rv.begin()+ii,rv.end()-jj).

template<class v>
Range<v> drop(Range<v> rv,index ii,index jj)
{
   Assert(ii+jj <= rv.size());                           // Validate arguments

   return Range<v>(rv.begin()+ii,rv.end()-jj);           // Construct subrange
}


//* SUB RANGE (Range<v>,index,index)
//*
//* Return the subrange [rv.begin()+ii,rv.end()+jj).

template<class v>
Range<v> subrange(Range<v> rv,index ii,index jj)
{
   Assert(ii<=jj && jj <= rv.size());                    // Validate arguments

   return Range<v>(rv.begin()+ii,rv.begin()+jj);         // Construct subrange
}


//* VALID (Range<v>)
//*
//* Return true if the range 'rv' holds valid iterators; that is, values other
//* than those special values assigned by the default constructor. Notice that
//* it suffices to test only the begin() iterator, since either both are valid
//* or else neither are.

template<class v>
bool valid(Range<v> rv)
{
	Assert(rv.begin()!=rcast<Range<v>::iterator>(~0) || rv.empty());

   return (rv.begin() != rcast<Range<v>::iterator>(~0));	// Is iterator valid?
}


                   /***   P U B L I C    M A C R O S   ***/


//* MS RANGE (type,count)
//*
//* Allocate on the stack a range of 'count' objects of the specified type. Be
//* aware that the 'count' parameter is evaluated twice in the macro expansion
//* and so should almost certainly not include any kind of side effect.

#define msrange(type,count) \
   Range<type>(count,msnew(type,count))                  // Allocate on stack
