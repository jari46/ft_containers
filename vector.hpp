#include <memory>
  // std::allocator
  // std::allocator<T>::destroy: 객체의 소멸자를 호출한다.
  // std::uninitialized_fill_n(): 값만 복사하는 대신, 객체를 해당 위치에 생성한다. 초기화되지 않은 공간을 채운다.
  // std::uninitialized_copy(): 값만 복사하는 대신, 객체를 해당 위치에 생성한다. 초기화되지 않은 공간에 복사한다.

#include <iterator>
  // std::distance(): first부터 last까지 몇 칸인지 반환한다.

#include <limits>
  // std::numeric_limits

#include "iterator.hpp"
  // ft::__normal_iterator

namespace ft
{

template <typename _Type, typename _AllocatorType>
class _Vector_base {

// vector base의 목적은, 
// vector base의 생성자와 소멸자가 vector의 공간할당을 담당하도록 하는 것이다. (초기화 제외)
// 이렇게 함으로써  예외 안정성을 더 쉽게 달성할 수 있다.

public:
  typedef _AllocatorType allocator_type; // std::allocator<_Type>가 default이다.
  
  // get_allocator()는 관련 할당기를 반환한다.
  allocator_type get_allocator() const { return _M_data_allocator; }

protected:
  allocator_type _M_data_allocator; // 할당기의 인스턴스화. 메모리를 할당/해제할 때, 메모리에 요소를 생성/소멸할 때 사용된다.
  _Type* _M_start;
  _Type* _M_finish; // 마지막 data의 다음
  _Type* _M_end_of_storage; // 마지막 저장가능한 공간의 다음

  _Type* // 할당된 공간의 첫번째 요소에 대한 포인터
    _M_allocate(size_t __n) { return _M_data_allocator.allocate(__n); }
  void _M_deallocate(_Type* __p, size_t __n) { if (__p) _M_data_allocator.deallocate(__p, __n); }

public:
  // CONSTRUCTOR
  _Vector_base(const allocator_type& __a)
    : _M_data_allocator(__a), _M_start(0), _M_finish(0), _M_end_of_storage(0) {}

  _Vector_base(size_t __n, const allocator_type& __a)
    : _M_data_allocator(__a), _M_start(_M_allocate(__n)), _M_finish(_M_start), _M_end_of_storage(_M_start + __n) {}

  // DESTRUCTOR
  ~_Vector_base() { _M_deallocate(_M_start, _M_end_of_storage - _M_start); }
};

template <
  typename _Type, // 요소의 타입
  typename _AllocatorType = std::allocator<_Type> // 할당기 객체의 타입
> class vector : protected _Vector_base<_Type, _AllocatorType>
{

// (멤버함수의 순서는 cppreference.com의 순서를 따라 정렬했습니다.)

private:
  typedef _Vector_base<_Type, _AllocatorType> _Base;
  typedef vector<_Type, _AllocatorType> vector_type;

public:
  // Member types

  typedef _Type  value_type; // 첫번째 템플릿 인수
  typedef _AllocatorType  allocator_type; // 두번째 템플릿 인수

  // 부호없는 정수 타입. (size_t는 unsigned long의 typedef다.)
  // defference_type의 부호없는 값을 나타낼 수 있다. 
  typedef std::size_t  size_type;

  // 부호있는 정수 타입. (ptrdiff_t는 long int의 typedef다.)
  // iterator_traits<iterator>::difference_type와 일치한다. 
  // (vector 클래스 내부에서는 사용되지 않았다.)
  typedef std::ptrdiff_t  difference_type;

  typedef value_type& reference;
  typedef const value_type&  const_reference;

  typedef typename  allocator_type::pointer pointer;
  typedef const typename  allocator_type::const_pointer const_pointer;

   typedef __normal_iterator<pointer, vector_type>  iterator; // 임의접근반복자. const iterator로 변형이 가능하다.
   typedef __normal_iterator<const_pointer, vector_type>  const_iterator; //임의접근반복자.

   typedef std::reverse_iterator<iterator>  reverse_iterator;
   typedef std::reverse_iterator<const_iterator>  const_reverse_iterator;

protected:
  using _Base::_M_allocate;
  using _Base::_M_deallocate;
  using _Base::_M_start;
  using _Base::_M_finish;
  using _Base::_M_end_of_storage;

public:
  // 멤버함수: 요소 접근 관련

  // at() :
  // 벡터에서 위치 __n에 있는 요소에 대한 참조를 반환한다.
  // 자동적으로 __n이 범위 내에 있는지 확인하고, 아닐 경우 out_of_range를 던진다. 범위 확인을 하지 않는 operator[]와 대조된다.
  // Exception safety: strong guarantee. 
  reference at(size_type __n) { _M_range_check(__n); return (*this)[__n]; }
  const_reference at(size_type __n) const { _M_range_check(__n); return (*this)[__n]; } // 벡터 객체가 const-qualified인 경우

  // operator[] :
  // at과 동일하되, 범위 확인을 하지 않는다.
  // 범위 밖의 위치를 요구했을 때는 out_of_range exception을 던지지만, undefined behavior이므로 이식성을 고려한다면 사용하지 않아야 한다.
  // Exception safety: 벡터의 size가 __n보다 클 경우 no-throw guarantee, 나머지 경우는 undefined behavior. 
  reference operator[](size_type __n) { return *(begin() + __n); }
  const_reference operator[](size_type __n) const { return *(begin() + __n); }

  // front() :
  // vector의 첫번째 요소에 대한 참조를 반환한다.
  reference front() { return *begin(); }
  const_reference front() const { return *begin(); }

  // back() :
  // vector의 마지막 요소에 대한 참조를 반환한다.
  reference back() { return *(end() - 1); }
  const_reference back() const { return *(end() - 1); }

  // data() :
  // vector가 내부적으로 사용하는 메모리 배열에 대한 포인터를 반환한다.
  value_type* data() { return _M_start; }
  value_type* data() const { return _M_start; } // 벡터 객체가 const-qualified인 경우

public:
  // 멤버함수: 반복자 관련

  // begin() :
  // 첫번째 요소를 가리키는 반복자를 반환한다.
  iterator begin() { return iterator (_M_start); }
  const_iterator begin() const { return const_iterator (_M_start); }

  // end() :
  // '마지막 요소의 다음 요소'을 가리키는 반복자를 반환한다.
  iterator end() { return iterator (_M_finish); }
  const_iterator end() const { return const_iterator (_M_finish); }

  // rbegin() :
  // 마지막 요소를 가리키는 역반복자를 반환한다.
  // (역반복자 입장에서는 마지막 요소가 첫번째 요소다.)
  reverse_iterator rbegin() { return reverse_iterator(end()); }
  const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }
  
  // rend() :
  // '첫번째 요소의 (이론적) 앞 요소'을 가리키는 반복자를 반환한다.
  // (역반복자 입장에서는 '첫번째 요소의 앞 요소'가 '마지막 요소의 다음 요소'다.)
  reverse_iterator rend() { return reverse_iterator(begin()); }
  const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }

 public:
 // 멤버함수: capacity 관련

  // empty() :
  // vector가 비었으면(즉 size가 0이면) true, 아니면 false를 반환한다.
  bool empty() const { return begin() == end(); }

  // size() :
  // vector의 요소의 수를 반환한다.
  size_type size() const { return size_type(end() - begin()); }

  // max_size() :
  // vector가 저장할 수 있는 요소의 최대의 개수를 반환한다. 단, 그 크기에 도달하기 전에 어느 시점에서든지 할당이 불가능해질 수 있다.
  // Q. 왜 std::numeric_limits<difference_type>::max()를 사용했나?
  // A. 만약 32-bit 시스템이라면, 이론적으로 4Gb == 2^32bits만큼 할당할 수 있다.
  //    이 수는 가능한 주소값의 개수로 구할 수 있는데, 주소값의 개수가 곧 할당할 수 있는 공간의 개수이기 때문이다.
  size_type max_size() const { return std::numeric_limits<difference_type>::max() / sizeof(_Type); }

  // reserve() :
  // 새로 할당 → 원래 내용을 새 공간에 복사 → 원래 메모리를 destroy, deallocate → 새로운 메모리를 start에 저장 순으로 동작한다.
  void reserve(size_type __n) {
    if (capacity() < __n) {
      const size_type __old_size = size();
      pointer __tmp = _M_allocate_and_copy(__n, _M_start, _M_finish);
      _M_destroy(_M_start, _M_finish);
      _M_deallocate(_M_start, _M_end_of_storage - _M_start);
      _M_start = __tmp;
      _M_finish = __tmp + __old_size;
      _M_end_of_storage = _M_start + __n;
    }
  }

  // returns the number of elements that can be held in currently allocated storage
  size_type capacity() const { return size_type(const_iterator(_M_end_of_storage) - begin()); }

 public:
  // Member functions: Modifiers

  // clears the contents
  void clear() { erase(begin(), end()); }

  // inserts elements

  // insert1. single element
  // return an iterator that points to the first of the newly inserted elements.
  iterator insert(
    iterator __position, // iterator: defined as a random access iterator type
    const _Type& __x) {
    size_type __n = __position - begin();
    if (_M_finish != _M_end_of_storage && __position == end()) { // inserts a single element at the end
      _M_data_allocator.construct(_M_finish, __x);
      ++_M_finish;
    }
    else
      _M_insert_aux(iterator(__position), __x);
    return begin() + __n;
  }
  // insert2. fill
  void insert (iterator __pos, size_type __n, const _Type& __x)
    { _M_fill_insert(__pos, __n, __x); }

    // insert3. range
    // Check whether it's an integral type.  If so, it's not an iterator.
   template<class _InputIterator>
     void insert(iterator __pos, _InputIterator __first, _InputIterator __last) {
       typedef typename _Is_integer<_InputIterator>::_Integral _Integral;
       _M_insert_dispatch(__pos, __first, __last, _Integral());
     }

   template <class _Integer>
     void _M_insert_dispatch(iterator __pos, _Integer __n, _Integer __val, __true_type)
     { _M_fill_insert(__pos, static_cast<size_type>(__n), static_cast<_Type>(__val)); }

   template<class _InputIterator>
     void _M_insert_dispatch(iterator __pos, _InputIterator __first, _InputIterator __last, __false_type) {
     typedef typename iterator_traits<_InputIterator>::iterator_category _IterCategory;
       _M_range_insert(__pos, __first, __last, _IterCategory());
     }

   // erases elements
   iterator erase(iterator __position) {
     if (__position + 1 != end())
       copy(__position + 1, end(), __position);
     --_M_finish;
     _M_destroy(_M_finish);
     return __position;
   }
   iterator erase(iterator __first, iterator __last) {
     iterator __i(copy(__last, end(), __first));
     _M_destroy(__i, end());
     _M_finish = _M_finish - (__last - __first);
     return __first;
   }

   // adds an element to the end
   void push_back(const _Type& __x) {
     if (_M_finish != _M_end_of_storage) {
       _Construct(_M_finish, __x);
       ++_M_finish;
     }
     else
       _M_insert_aux(end(), __x);
   }

   void push_back() {
     if (_M_finish != _M_end_of_storage) {
       _M_data_allocator.construct(_M_finish);
       ++_M_finish;
     }
     else
       _M_insert_aux(end());
   }

   // removes the last element
   void pop_back() {
     --_M_finish;
     _M_destroy(_M_finish);
   }

   // changes the number of elements stored
   void resize(size_type __new_size, const _Type& __x) {
     if (__new_size < size())
       erase(begin() + __new_size, end());
     else
       insert(end(), __new_size - size(), __x);
   }
   void resize(size_type __new_size) { resize(__new_size, _Type()); }

   // swaps the contents
   void
   swap(vector<_Type, _Alloc>& __x)
   {
     std::swap(_M_start, __x._M_start);
     std::swap(_M_finish, __x._M_finish);
     std::swap(_M_end_of_storage, __x._M_end_of_storage);
   }

public:
  // CONSTRUCTOR

  // [Exception safety: Strong guarantee]
  // : Constructs a vector, initializing its contents depending on the constructor version used.

  // constructor1. empty container constructor (default constructor)
  explicit vector(
    const allocator_type& __a = allocator_type() // Allocator object. The container keeps and uses an internal copy of this allocator.
  ) : _Base(__a) {}

  // constructor2. fill constructor
  // : Constructs a container with n elements. Each element is a copy of __value.
  explicit vector(
    size_type __n, // Initial container size
    const _Type& __value, // Value to fill the container with.
    const allocator_type& __a = allocator_type()
  ) : _Base(__n, __a)
    { _M_finish = std::uninitialized_fill_n(_M_start, __n, __value); }

  // constructor3. range constructor
  // (if the range specified by [__first, __last) is not valid, it causes undefined behavior.)
  template <class _InputIterator>
    vector(
      typename std::enable_if<!std::is_integral<_InputIterator>::value,// Check whether it's an integral type.  If so, it's not an iterator (then it choosees constructor2 by SFINE).
      _InputIterator>::type  __first,
      _InputIterator __last,
      const allocator_type& __a = allocator_type()
    ) : _Base(__a) {
    typedef typename std::iterator_traits<_InputIterator>::iterator_category __IteratorCategory;
    _M_range_initialize(__first, __last, __IteratorCategory());
  }

  // constructor4. copy constructor
  vector(
    const vector& __x // Another vector object of the same type (with the same class template arguments T and Alloc),
  ) : _Base(__x.size(), __x.get_allocator())
    { _M_finish = std::uninitialized_copy(__x.begin(), __x.end(), _M_start); }

  // DESTRUCTOR
  ~vector()
  { _M_destroy(_M_start, _M_finish); }

  // ASSIGN OPERATOR
  vector<_Type, _Alloc>& operator=(const vector<_Type, _Alloc>& __x);

  // ASSIGN FUNCTION

  // assign(), a generalized assignment member function.  Two
  // versions: one that takes a count, and one that takes a range.
  // The range version is a member template, so we dispatch on whether
  // or not the type is an integer.

  void assign(size_type __n, const _Type& __val) { _M_fill_assign(__n, __val); }
  void _M_fill_assign(size_type __n, const _Type& __val);

  template<class _InputIterator>
    void
  assign(_InputIterator __first, _InputIterator __last)
  {
      typedef typename _Is_integer<_InputIterator>::_Integral _Integral;
      _M_assign_dispatch(__first, __last, _Integral());
    }

  template<class _Integer>
    void
  _M_assign_dispatch(_Integer __n, _Integer __val, __true_type)
    { _M_fill_assign((size_type) __n, (_Type) __val); }

  template<class _InputIter>
    void
  _M_assign_dispatch(_InputIter __first, _InputIter __last, __false_type)
    {
    typedef typename iterator_traits<_InputIter>::iterator_category _IterCategory;
    _M_assign_aux(__first, __last, _IterCategory());
  }

  template <class _InputIterator>
  void _M_assign_aux(_InputIterator __first, _InputIterator __last,
                    input_iterator_tag);

  template <class _ForwardIterator>
  void _M_assign_aux(_ForwardIterator __first, _ForwardIterator __last,
                    forward_iterator_tag);

  //ALLOCATOR
  // returns the associated allocator
  allocator_type get_allocator() const { return _Base::get_allocator(); }


//----------------------------------------------------------------------------------------

protected:
  // _M_FUNCTIONS

  // _M_destroy()

  template <typename _ForwardIterator>
  void _M_destroy(_ForwardIterator __first, _ForwardIterator __last) {
    for (; __first != last; ++first)
      _M_data_allocator.destroy(__first);
  }

  template <typename _ForwardIterator>
  void _M_destory(_ForwardIterator __position) {
      _M_data_allocator.destroy(__position);
  }

  // _M_range_check() :
  // at() 함수가 사용하는 함수다.
    void _M_range_check(size_type __n) const {
      if (__n >= this->size())
        __throw_out_of_range("vector");
    }

  // _M_range_initialize() :
  // constructor3만 사용하는 함수다.
    template <class _InputIterator>
    void _M_range_initialize(_InputIterator __first, _InputIterator __last, std::input_iterator_tag) {
      for ( ; __first != __last; ++__first) // forward iterator가 아닐 경우 하나하나 할당해주어야 한다.
        push_back(*__first);
    }

    // The forward_iterator_tag struct and its derived structs
    template <class _ForwardIterator>
    void _M_range_initialize(_ForwardIterator __first, _ForwardIterator __last, std::forward_iterator_tag) {
      size_type __n = std::distance(__first, __last);
      _M_start = _M_allocate(__n);
      _M_end_of_storage = _M_start + __n;
      _M_finish = std::uninitialized_copy(__first, __last, _M_start);
    }

  template <class _ForwardIterator>
  pointer _M_allocate_and_copy(size_type __n, _ForwardIterator __first, _ForwardIterator __last) {
    pointer __result = _M_allocate(__n);
    try {
      std::uninitialized_copy(__first, __last, __result);
      return __result;
    }
    catch(...) {
      _M_deallocate(__result, __n);
      throw; // (try에서 발생한 예외를 재발생시킨다.)
    }
  }

   void _M_insert_aux(iterator __position, const _Type& __x);
   void _M_insert_aux(iterator __position);

   template <class _InputIterator>
   void _M_range_insert(iterator __pos,
                        _InputIterator __first, _InputIterator __last,
                        input_iterator_tag);

   template <class _ForwardIterator>
   void _M_range_insert(iterator __pos,
                        _ForwardIterator __first, _ForwardIterator __last,
                        forward_iterator_tag);
 };

 template <class _Type, class _Alloc>
 inline bool
 operator==(const vector<_Type, _Alloc>& __x, const vector<_Type, _Alloc>& __y)
 {
   return __x.size() == __y.size() &&
          equal(__x.begin(), __x.end(), __y.begin());
 }

 template <class _Type, class _Alloc>
 inline bool
 operator<(const vector<_Type, _Alloc>& __x, const vector<_Type, _Alloc>& __y)
 {
   return lexicographical_compare(__x.begin(), __x.end(),
                                  __y.begin(), __y.end());
 }

 template <class _Type, class _Alloc>
 inline void swap(vector<_Type, _Alloc>& __x, vector<_Type, _Alloc>& __y)
 {
   __x.swap(__y);
 }

 template <class _Type, class _Alloc>
 inline bool
 operator!=(const vector<_Type, _Alloc>& __x, const vector<_Type, _Alloc>& __y) {
   return !(__x == __y);
 }

 template <class _Type, class _Alloc>
 inline bool
 operator>(const vector<_Type, _Alloc>& __x, const vector<_Type, _Alloc>& __y) {
   return __y < __x;
 }

 template <class _Type, class _Alloc>
 inline bool
 operator<=(const vector<_Type, _Alloc>& __x, const vector<_Type, _Alloc>& __y) {
   return !(__y < __x);
 }

 template <class _Type, class _Alloc>
 inline bool
 operator>=(const vector<_Type, _Alloc>& __x, const vector<_Type, _Alloc>& __y) {
   return !(__x < __y);
 }

 template <class _Type, class _Alloc>
 vector<_Type,_Alloc>&
 vector<_Type,_Alloc>::operator=(const vector<_Type, _Alloc>& __x)
 {
   if (&__x != this) {
     const size_type __xlen = __x.size();
     if (__xlen > capacity()) {
       pointer __tmp = _M_allocate_and_copy(__xlen, __x.begin(), __x.end());
       _M_destroy(_M_start, _M_finish);
       _M_deallocate(_M_start, _M_end_of_storage - _M_start);
       _M_start = __tmp;
       _M_end_of_storage = _M_start + __xlen;
     }
     else if (size() >= __xlen) {
       iterator __i(copy(__x.begin(), __x.end(), begin()));
       _M_destroy(__i, end());
     }
     else {
       copy(__x.begin(), __x.begin() + size(), _M_start);
       std::uninitialized_copy(__x.begin() + size(), __x.end(), _M_finish);
     }
     _M_finish = _M_start + __xlen;
   }
   return *this;
 }

 template <class _Type, class _Alloc>
 void vector<_Type, _Alloc>::_M_fill_assign(size_t __n, const value_type& __val)
 {
   if (__n > capacity()) {
     vector<_Type, _Alloc> __tmp(__n, __val, get_allocator());
     __tmp.swap(*this);
   }
   else if (__n > size()) {
     fill(begin(), end(), __val);
     _M_finish = std::uninitialized_fill_n(_M_finish, __n - size(), __val);
   }
   else
     erase(fill_n(begin(), __n, __val), end());
 }

 template <class _Type, class _Alloc> template <class _InputIter>
 void vector<_Type, _Alloc>::_M_assign_aux(_InputIter __first, _InputIter __last,
                                         input_iterator_tag) {
   iterator __cur(begin());
   for ( ; __first != __last && __cur != end(); ++__cur, ++__first)
     *__cur = *__first;
   if (__first == __last)
     erase(__cur, end());
   else
     insert(end(), __first, __last);
 }

 template <class _Type, class _Alloc> template <class _ForwardIter>
 void
 vector<_Type, _Alloc>::_M_assign_aux(_ForwardIter __first, _ForwardIter __last,
                                    forward_iterator_tag) {
   size_type __len = 0;
   std::distance(__first, __last, __len);

   if (__len > capacity()) {
     pointer __tmp(_M_allocate_and_copy(__len, __first, __last));
     _M_destroy(_M_start, _M_finish);
     _M_deallocate(_M_start, _M_end_of_storage - _M_start);
     _M_start = __tmp;
     _M_end_of_storage = _M_finish = _M_start + __len;
   }
   else if (size() >= __len) {
     iterator __new_finish(copy(__first, __last, _M_start));
     _M_destroy(__new_finish, end());
     _M_finish = __new_finish.base();
   }
   else {
     _ForwardIter __mid = __first;
     advance(__mid, size());
     copy(__first, __mid, _M_start);
     _M_finish = std::uninitialized_copy(__mid, __last, _M_finish);
   }
 }

 template <class _Type, class _Alloc>
 void vector<_Type, _Alloc>::_M_insert_aux(iterator __position, const _Type& __x)
 {
   if (_M_finish != _M_end_of_storage) {
     _Construct(_M_finish, *(_M_finish - 1));
     ++_M_finish;
     _Type __x_copy = __x;
     copy_backward(__position, iterator(_M_finish - 2), iterator(_M_finish- 1));
     *__position = __x_copy;
   }
   else {
     const size_type __old_size = size();
     const size_type __len = __old_size != 0 ? 2 * __old_size : 1;
     iterator __new_start(_M_allocate(__len));
     iterator __new_finish(__new_start);
     try {
       __new_finish = std::uninitialized_copy(iterator(_M_start), __position,
                                         __new_start);
       _Construct(__new_finish.base(), __x);
       ++__new_finish;
       __new_finish = std::uninitialized_copy(__position, iterator(_M_finish),
                                         __new_finish);
     }
     catch(...)
       {
 	_M_destroy(__new_start,__new_finish);
 	_M_deallocate(__new_start.base(),__len);
 	__throw_exception_again;
       }
     _M_destroy(begin(), end());
     _M_deallocate(_M_start, _M_end_of_storage - _M_start);
     _M_start = __new_start.base();
     _M_finish = __new_finish.base();
     _M_end_of_storage = __new_start.base() + __len;
   }
 }

 template <class _Type, class _Allocator>
 void vector<_Type, _Allocator>::_M_insert_aux(iterator __position)
 {
   if (_M_finish != _M_end_of_storage) {
     _Construct(_M_finish, *(_M_finish - 1));
     ++_M_finish;
     copy_backward(__position, iterator(_M_finish - 2),
 		  iterator(_M_finish - 1));
     *__position = _Type();
   }
   else {
     const size_type __old_size = size();
     const size_type __len = __old_size != 0 ? 2 * __old_size : 1;
     pointer __new_start = _M_allocate(__len);
     pointer __new_finish = __new_start;
     try {
       __new_finish = std::uninitialized_copy(iterator(_M_start), __position,
 					__new_start);
       _Construct(__new_finish);
       ++__new_finish;
       __new_finish = std::uninitialized_copy(__position, iterator(_M_finish),
 					__new_finish);
     }
     catch(...)
       {
 	_M_destroy(__new_start,__new_finish);
 	_M_deallocate(__new_start,__len);
 	__throw_exception_again;
       }
     _M_destroy(begin(), end());
     _M_deallocate(_M_start, _M_end_of_storage - _M_start);
     _M_start = __new_start;
     _M_finish = __new_finish;
     _M_end_of_storage = __new_start + __len;
   }
 }

 template <class _Type, class _Alloc>
 void vector<_Type, _Alloc>::_M_fill_insert(iterator __position, size_type __n,
                                          const _Type& __x)
 {
   if (__n != 0) {
     if (size_type(_M_end_of_storage - _M_finish) >= __n) {
       _Type __x_copy = __x;
       const size_type __elems_after = end() - __position;
       iterator __old_finish(_M_finish);
       if (__elems_after > __n) {
         std::uninitialized_copy(_M_finish - __n, _M_finish, _M_finish);
         _M_finish += __n;
         copy_backward(__position, __old_finish - __n, __old_finish);
         fill(__position, __position + __n, __x_copy);
       }
       else {
         std::uninitialized_fill_n(_M_finish, __n - __elems_after, __x_copy);
         _M_finish += __n - __elems_after;
         std::uninitialized_copy(__position, __old_finish, _M_finish);
         _M_finish += __elems_after;
         fill(__position, __old_finish, __x_copy);
       }
     }
     else {
       const size_type __old_size = size();
       const size_type __len = __old_size + max(__old_size, __n);
       iterator __new_start(_M_allocate(__len));
       iterator __new_finish(__new_start);
       try {
         __new_finish = std::uninitialized_copy(begin(), __position, __new_start);
         __new_finish = std::uninitialized_fill_n(__new_finish, __n, __x);
         __new_finish
           = std::uninitialized_copy(__position, end(), __new_finish);
       }
       catch(...)
 	{
 	  _M_destroy(__new_start,__new_finish);
 	  _M_deallocate(__new_start.base(),__len);
 	  __throw_exception_again;
 	}
       _M_destroy(_M_start, _M_finish);
       _M_deallocate(_M_start, _M_end_of_storage - _M_start);
       _M_start = __new_start.base();
       _M_finish = __new_finish.base();
       _M_end_of_storage = __new_start.base() + __len;
     }
   }
 }

 template <class _Type, class _Alloc> template <class _InputIterator>
 void
 vector<_Type, _Alloc>::_M_range_insert(iterator __pos,
                                      _InputIterator __first,
                                      _InputIterator __last,
                                      input_iterator_tag)
 {
   for ( ; __first != __last; ++__first) {
     __pos = insert(__pos, *__first);
     ++__pos;
   }
 }

 template <class _Type, class _Alloc> template <class _ForwardIterator>
 void
 vector<_Type, _Alloc>::_M_range_insert(iterator __position,
                                      _ForwardIterator __first,
                                      _ForwardIterator __last,
                                      forward_iterator_tag)
 {
   if (__first != __last) {
     size_type __n = 0;
     std::distance(__first, __last, __n);
     if (size_type(_M_end_of_storage - _M_finish) >= __n) {
       const size_type __elems_after = end() - __position;
       iterator __old_finish(_M_finish);
       if (__elems_after > __n) {
         std::uninitialized_copy(_M_finish - __n, _M_finish, _M_finish);
         _M_finish += __n;
         copy_backward(__position, __old_finish - __n, __old_finish);
         copy(__first, __last, __position);
       }
       else {
         _ForwardIterator __mid = __first;
         advance(__mid, __elems_after);
         std::uninitialized_copy(__mid, __last, _M_finish);
         _M_finish += __n - __elems_after;
         std::uninitialized_copy(__position, __old_finish, _M_finish);
         _M_finish += __elems_after;
         copy(__first, __mid, __position);
       }
     }
     else {
       const size_type __old_size = size();
       const size_type __len = __old_size + max(__old_size, __n);
       iterator __new_start(_M_allocate(__len));
       iterator __new_finish(__new_start);
       try {
         __new_finish = std::uninitialized_copy(iterator(_M_start),
 					  __position, __new_start);
         __new_finish = std::uninitialized_copy(__first, __last, __new_finish);
         __new_finish
           = std::uninitialized_copy(__position, iterator(_M_finish), __new_finish);
       }
       catch(...)
 	{
 	  _M_destroy(__new_start,__new_finish);
 	  _M_deallocate(__new_start.base(), __len);
 	  __throw_exception_again;
 	}
       _M_destroy(_M_start, _M_finish);
       _M_deallocate(_M_start, _M_end_of_storage - _M_start);
       _M_start = __new_start.base();
       _M_finish = __new_finish.base();
       _M_end_of_storage = __new_start.base() + __len;
     }
   }
};

} // namespace ft
