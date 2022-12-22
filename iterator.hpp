#include "iterator_base_types.hpp"
	// ft::iterator_traits

// normal_iterator
// 1) it does not change the semantics of any of the operators of its iterator parameter.
//
// 2) Its primary purpose is to convert an iterator that is not a class, into an iterator that is a class.
//		e.g. a pointer
//
// 3) The _Container parameter exists solely so that different containers
// 		using this template can instantiate different types, even if the _Iterator parameter is the same.
//		e.g. _normal_iterator< int*, vector<int> > != _normal_iterator< int*, basic_string >

namespace ft
{
  template<typename _Iterator, typename _Container>
    class __normal_iterator
      : public iterator<typename iterator_traits<_Iterator>::iterator_category,
                        typename iterator_traits<_Iterator>::value_type,
                        typename iterator_traits<_Iterator>::difference_type,
                        typename iterator_traits<_Iterator>::pointer,
                        typename iterator_traits<_Iterator>::reference>
    {
    protected:
      _Iterator _M_current;

    public:
      typedef typename iterator_traits<_Iterator>::difference_type	difference_type;
      typedef typename iterator_traits<_Iterator>::reference	reference;
      typedef typename iterator_traits<_Iterator>::pointer	pointer;

			// default constructor
      __normal_iterator() : _M_current(_Iterator()) { }

			// copy constructor
      explicit __normal_iterator(const _Iterator& __i) : _M_current(__i) { }

      // copy constructor: Allow iterator to const_iterator conversion //질문:???
      template<typename _Iter>
      inline __normal_iterator(const __normal_iterator<_Iter, _Container>& __i) : _M_current(__i.base()) { }

			// copy assignable: default
			// destructable: default

		public:
      // Forward iterator requirements

      // DEREFERENCE
			// can be dereferenced as an rvalue or as an lvalue (for non-const iterators)
			// e.g. a = *iter; *iter = a;
			reference operator*() const { return *_M_current; }

      // can be dereferenced as an rvalue
			// use if the type is structure or union. translate as (*pointer).m
			// e.g. iter->m;
      pointer operator->() const { return _M_current; }

			// INCRESMENT
			// The result is either also dereferenceable or a past-the-end iterator
      __normal_iterator& operator++() { ++_M_current; return *this; }

      __normal_iterator operator++(int) { return __normal_iterator(_M_current++); }

		// public:
    //   // Bidirectional iterator requirements

		//   __normal_iterator&
    //   operator--() { --_M_current; return *this; }

    //   __normal_iterator
    //   operator--(int) { return __normal_iterator(_M_current--); }

    //   // Random access iterator requirements
    //   reference
    //   operator[](const difference_type& __n) const
    //   { return _M_current[__n]; }

    //   __normal_iterator&
    //   operator+=(const difference_type& __n)
    //   { _M_current += __n; return *this; }

    //   __normal_iterator
    //   operator+(const difference_type& __n) const
    //   { return __normal_iterator(_M_current + __n); }

    //   __normal_iterator&
    //   operator-=(const difference_type& __n)
    //   { _M_current -= __n; return *this; }

    //   __normal_iterator
    //   operator-(const difference_type& __n) const
    //   { return __normal_iterator(_M_current - __n); }

    //   difference_type
    //   operator-(const __normal_iterator& __i) const
    //   { return _M_current - __i._M_current; }

      const _Iterator&
      base() const { return _M_current; }
    };

// normal iterator operator
// they shoulde be 1) normal iterator type, 2) same container type to use this operators

	// equality/unequality
  // : this is Forward iterator requirements
	// (meaningful when both iterator values iterate over the same underlying sequence)

  // template<typename _IteratorL, typename _IteratorR, typename _Container>
  // inline bool operator==(const __normal_iterator<_IteratorL, _Container>& __lhs, const __normal_iterator<_IteratorR, _Container>& __rhs)
  // { return __lhs.base() == __rhs.base(); }

  // template<typename _IteratorL, typename _IteratorR, typename _Container>
  // inline bool operator!=(const __normal_iterator<_IteratorL, _Container>& __lhs, const __normal_iterator<_IteratorR, _Container>& __rhs)
  // { return !(__lhs == __rhs); }

  // // Random access iterator requirements
  // template<typename _IteratorL, typename _IteratorR, typename _Container>
  // inline bool operator<(const __normal_iterator<_IteratorL, _Container>& __lhs, const __normal_iterator<_IteratorR, _Container>& __rhs)
  // { return __lhs.base() < __rhs.base(); }

  // template<typename _IteratorL, typename _IteratorR, typename _Container>
  // inline bool operator>(const __normal_iterator<_IteratorL, _Container>& __lhs, const __normal_iterator<_IteratorR, _Container>& __rhs)
  // { return __rhs < __lhs; }

  // template<typename _IteratorL, typename _IteratorR, typename _Container>
  // inline bool operator<=(const __normal_iterator<_IteratorL, _Container>& __lhs, const __normal_iterator<_IteratorR, _Container>& __rhs)
  // { return !(__rhs < __lhs); }

  // template<typename _IteratorL, typename _IteratorR, typename _Container>
  // inline bool operator>=(const __normal_iterator<_IteratorL, _Container>& __lhs, const __normal_iterator<_IteratorR, _Container>& __rhs)
  // { return !(__lhs < __rhs); }

  // template<typename _Iterator, typename _Container>
  // inline __normal_iterator<_Iterator, _Container>
  // operator+(typename __normal_iterator<_Iterator, _Container>::difference_type __n,
	//     const __normal_iterator<_Iterator, _Container>& __i)
  // { return __normal_iterator<_Iterator, _Container>(__i.base() + __n); }

} // namespace ft