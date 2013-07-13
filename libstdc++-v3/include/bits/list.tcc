// List implementation (out of line) -*- C++ -*-

// Copyright (C) 2001-2013 Free Software Foundation, Inc.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 3, or (at your option)
// any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// Under Section 7 of GPL version 3, you are granted additional
// permissions described in the GCC Runtime Library Exception, version
// 3.1, as published by the Free Software Foundation.

// You should have received a copy of the GNU General Public License and
// a copy of the GCC Runtime Library Exception along with this program;
// see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
// <http://www.gnu.org/licenses/>.

/*
 *
 * Copyright (c) 1994
 * Hewlett-Packard Company
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  Hewlett-Packard Company makes no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 *
 *
 * Copyright (c) 1996,1997
 * Silicon Graphics Computer Systems, Inc.
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  Silicon Graphics makes no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 */

/** @file bits/list.tcc
 *  This is an internal header file, included by other library headers.
 *  Do not attempt to use it directly. @headername{list}
 */

#ifndef _LIST_TCC
#define _LIST_TCC 1

namespace std _GLIBCXX_VISIBILITY(default)
{
_GLIBCXX_BEGIN_NAMESPACE_CONTAINER

  template<typename _Tp, typename _Alloc>
    void
    _List_base<_Tp, _Alloc>::
    _M_clear()
    {
      typedef _List_node<_Tp>  _Node;
      _Node* __cur = static_cast<_Node*>(_M_impl._M_node._M_next);
      while (__cur != &_M_impl._M_node)
	{
	  _Node* __tmp = __cur;
	  __cur = static_cast<_Node*>(__cur->_M_next);
#if __cplusplus >= 201103L
	  _M_get_Node_allocator().destroy(__tmp);
#else
	  _M_get_Tp_allocator().destroy(std::__addressof(__tmp->_M_data));
#endif
	  _M_put_node(__tmp);
	}
    }

#if __cplusplus >= 201103L
  template<typename _Tp, typename _Alloc>
    template<typename... _Args>
      typename list<_Tp, _Alloc>::iterator
      list<_Tp, _Alloc>::
      emplace(const_iterator __position, _Args&&... __args)
      {
	_Node* __tmp = _M_create_node(std::forward<_Args>(__args)...);
	__tmp->_M_hook(__position._M_const_cast()._M_node);
	return iterator(__tmp);
      }
#endif

  template<typename _Tp, typename _Alloc>
    typename list<_Tp, _Alloc>::iterator
    list<_Tp, _Alloc>::
#if __cplusplus >= 201103L
    insert(const_iterator __position, const value_type& __x)
#else
    insert(iterator __position, const value_type& __x)
#endif
    {
      _Node* __tmp = _M_create_node(__x);
      __tmp->_M_hook(__position._M_const_cast()._M_node);
      return iterator(__tmp);
    }

#if __cplusplus >= 201103L
  template<typename _Tp, typename _Alloc>
    typename list<_Tp, _Alloc>::iterator
    list<_Tp, _Alloc>::
    insert(const_iterator __position, size_type __n, const value_type& __x)
    {
      if (__n)
	{
	  list __tmp(__n, __x, get_allocator());
	  iterator __it = __tmp.begin();
	  splice(__position, __tmp);
	  return __it;
	}
      return __position._M_const_cast();
    }

  template<typename _Tp, typename _Alloc>
    template<typename _InputIterator, typename>
      typename list<_Tp, _Alloc>::iterator
      list<_Tp, _Alloc>::
      insert(const_iterator __position, _InputIterator __first,
	     _InputIterator __last)
      {
	list __tmp(__first, __last, get_allocator());
	if (!__tmp.empty())
	  {
	    iterator __it = __tmp.begin();
	    splice(__position, __tmp);
	    return __it;
	  }
	return __position._M_const_cast();
      }
#endif

  template<typename _Tp, typename _Alloc>
    typename list<_Tp, _Alloc>::iterator
    list<_Tp, _Alloc>::
#if __cplusplus >= 201103L
    erase(const_iterator __position)
#else
    erase(iterator __position)
#endif
    {
      iterator __ret = iterator(__position._M_node->_M_next);
      _M_erase(__position._M_const_cast());
      return __ret;
    }

#if __cplusplus >= 201103L
  template<typename _Tp, typename _Alloc>
    void
    list<_Tp, _Alloc>::
    _M_default_append(size_type __n)
    {
      size_type __i = 0;
      __try
	{
	  for (; __i < __n; ++__i)
	    emplace_back();
	}
      __catch(...)
	{
	  for (; __i; --__i)
	    pop_back();
	  __throw_exception_again;
	}
    }

  template<typename _Tp, typename _Alloc>
    void
    list<_Tp, _Alloc>::
    resize(size_type __new_size)
    {
      iterator __i = begin();
      size_type __len = 0;
      for (; __i != end() && __len < __new_size; ++__i, ++__len)
        ;
      if (__len == __new_size)
        erase(__i, end());
      else                          // __i == end()
	_M_default_append(__new_size - __len);
    }

  template<typename _Tp, typename _Alloc>
    void
    list<_Tp, _Alloc>::
    resize(size_type __new_size, const value_type& __x)
    {
      iterator __i = begin();
      size_type __len = 0;
      for (; __i != end() && __len < __new_size; ++__i, ++__len)
        ;
      if (__len == __new_size)
        erase(__i, end());
      else                          // __i == end()
        insert(end(), __new_size - __len, __x);
    }
#else
  template<typename _Tp, typename _Alloc>
    void
    list<_Tp, _Alloc>::
    resize(size_type __new_size, value_type __x)
    {
      iterator __i = begin();
      size_type __len = 0;
      for (; __i != end() && __len < __new_size; ++__i, ++__len)
        ;
      if (__len == __new_size)
        erase(__i, end());
      else                          // __i == end()
        insert(end(), __new_size - __len, __x);
    }
#endif

  template<typename _Tp, typename _Alloc>
    list<_Tp, _Alloc>&
    list<_Tp, _Alloc>::
    operator=(const list& __x)
    {
      if (this != &__x)
	{
	  iterator __first1 = begin();
	  iterator __last1 = end();
	  const_iterator __first2 = __x.begin();
	  const_iterator __last2 = __x.end();
	  for (; __first1 != __last1 && __first2 != __last2;
	       ++__first1, ++__first2)
	    *__first1 = *__first2;
	  if (__first2 == __last2)
	    erase(__first1, __last1);
	  else
	    insert(__last1, __first2, __last2);
	}
      return *this;
    }

  template<typename _Tp, typename _Alloc>
    void
    list<_Tp, _Alloc>::
    _M_fill_assign(size_type __n, const value_type& __val)
    {
      iterator __i = begin();
      for (; __i != end() && __n > 0; ++__i, --__n)
        *__i = __val;
      if (__n > 0)
        insert(end(), __n, __val);
      else
        erase(__i, end());
    }

  template<typename _Tp, typename _Alloc>
    template <typename _InputIterator>
      void
      list<_Tp, _Alloc>::
      _M_assign_dispatch(_InputIterator __first2, _InputIterator __last2,
			 __false_type)
      {
        iterator __first1 = begin();
        iterator __last1 = end();
        for (; __first1 != __last1 && __first2 != __last2;
	     ++__first1, ++__first2)
          *__first1 = *__first2;
        if (__first2 == __last2)
          erase(__first1, __last1);
        else
          insert(__last1, __first2, __last2);
      }

  template<typename _Tp, typename _Alloc>
    void
    list<_Tp, _Alloc>::
    remove(const value_type& __value)
    {
      iterator __first = begin();
      iterator __last = end();
      iterator __extra = __last;
      while (__first != __last)
	{
	  iterator __next = __first;
	  ++__next;
	  if (*__first == __value)
	    {
	      // _GLIBCXX_RESOLVE_LIB_DEFECTS
	      // 526. Is it undefined if a function in the standard changes
	      // in parameters?
	      if (std::__addressof(*__first) != std::__addressof(__value))
		_M_erase(__first);
	      else
		__extra = __first;
	    }
	  __first = __next;
	}
      if (__extra != __last)
	_M_erase(__extra);
    }

  template<typename _Tp, typename _Alloc>
    void
    list<_Tp, _Alloc>::
    unique()
    {
      iterator __first = begin();
      iterator __last = end();
      if (__first == __last)
	return;
      iterator __next = __first;
      while (++__next != __last)
	{
	  if (*__first == *__next)
	    _M_erase(__next);
	  else
	    __first = __next;
	  __next = __first;
	}
    }

  template<typename _Tp, typename _Alloc>
    void
    list<_Tp, _Alloc>::
#if __cplusplus >= 201103L
    merge(list&& __x)
#else
    merge(list& __x)
#endif
    {
      // _GLIBCXX_RESOLVE_LIB_DEFECTS
      // 300. list::merge() specification incomplete
      if (this != &__x)
	{
	  _M_check_equal_allocators(__x); 

	  iterator __first1 = begin();
	  iterator __last1 = end();
	  iterator __first2 = __x.begin();
	  iterator __last2 = __x.end();
	  while (__first1 != __last1 && __first2 != __last2)
	    if (*__first2 < *__first1)
	      {
		iterator __next = __first2;
		_M_transfer(__first1, __first2, ++__next);
		__first2 = __next;
	      }
	    else
	      ++__first1;
	  if (__first2 != __last2)
	    _M_transfer(__last1, __first2, __last2);
	}
    }

  template<typename _Tp, typename _Alloc>
    template <typename _StrictWeakOrdering>
      void
      list<_Tp, _Alloc>::
#if __cplusplus >= 201103L
      merge(list&& __x, _StrictWeakOrdering __comp)
#else
      merge(list& __x, _StrictWeakOrdering __comp)
#endif
      {
	// _GLIBCXX_RESOLVE_LIB_DEFECTS
	// 300. list::merge() specification incomplete
	if (this != &__x)
	  {
	    _M_check_equal_allocators(__x);

	    iterator __first1 = begin();
	    iterator __last1 = end();
	    iterator __first2 = __x.begin();
	    iterator __last2 = __x.end();
	    while (__first1 != __last1 && __first2 != __last2)
	      if (__comp(*__first2, *__first1))
		{
		  iterator __next = __first2;
		  _M_transfer(__first1, __first2, ++__next);
		  __first2 = __next;
		}
	      else
		++__first1;
	    if (__first2 != __last2)
	      _M_transfer(__last1, __first2, __last2);
	  }
      }

  template<typename _Tp, typename _Alloc>
    void
    list<_Tp, _Alloc>::
    sort()
    {
      // Do nothing if the list has length 0 or 1.
      if (this->_M_impl._M_node._M_next != &this->_M_impl._M_node
	  && this->_M_impl._M_node._M_next->_M_next != &this->_M_impl._M_node)
      {
        list __carry;
        list __tmp[64];
        list * __fill = &__tmp[0];
        list * __counter;

        do
	  {
	    __carry.splice(__carry.begin(), *this, begin());

	    for(__counter = &__tmp[0];
		__counter != __fill && !__counter->empty();
		++__counter)
	      {
		__counter->merge(__carry);
		__carry.swap(*__counter);
	      }
	    __carry.swap(*__counter);
	    if (__counter == __fill)
	      ++__fill;
	  }
	while ( !empty() );

        for (__counter = &__tmp[1]; __counter != __fill; ++__counter)
          __counter->merge(*(__counter - 1));
        swap( *(__fill - 1) );
      }
    }

  template<typename _Tp, typename _Alloc>
    template <typename _Predicate>
      void
      list<_Tp, _Alloc>::
      remove_if(_Predicate __pred)
      {
        iterator __first = begin();
        iterator __last = end();
        while (__first != __last)
	  {
	    iterator __next = __first;
	    ++__next;
	    if (__pred(*__first))
	      _M_erase(__first);
	    __first = __next;
	  }
      }

  template<typename _Tp, typename _Alloc>
    template <typename _BinaryPredicate>
      void
      list<_Tp, _Alloc>::
      unique(_BinaryPredicate __binary_pred)
      {
        iterator __first = begin();
        iterator __last = end();
        if (__first == __last)
	  return;
        iterator __next = __first;
        while (++__next != __last)
	  {
	    if (__binary_pred(*__first, *__next))
	      _M_erase(__next);
	    else
	      __first = __next;
	    __next = __first;
	  }
      }

  template<typename _Tp, typename _Alloc>
    template <typename _StrictWeakOrdering>
      void
      list<_Tp, _Alloc>::
      sort(_StrictWeakOrdering __comp)
      {
	// Do nothing if the list has length 0 or 1.
	if (this->_M_impl._M_node._M_next != &this->_M_impl._M_node
	    && this->_M_impl._M_node._M_next->_M_next != &this->_M_impl._M_node)
	  {
	    list __carry;
	    list __tmp[64];
	    list * __fill = &__tmp[0];
	    list * __counter;

	    do
	      {
		__carry.splice(__carry.begin(), *this, begin());

		for(__counter = &__tmp[0];
		    __counter != __fill && !__counter->empty();
		    ++__counter)
		  {
		    __counter->merge(__carry, __comp);
		    __carry.swap(*__counter);
		  }
		__carry.swap(*__counter);
		if (__counter == __fill)
		  ++__fill;
	      }
	    while ( !empty() );

	    for (__counter = &__tmp[1]; __counter != __fill; ++__counter)
	      __counter->merge(*(__counter - 1), __comp);
	    swap(*(__fill - 1));
	  }
      }

_GLIBCXX_END_NAMESPACE_CONTAINER
} // namespace std

#endif /* _LIST_TCC */

