// ============================================================================
// CS32 P01 Part 1: Vector implementation
//
// This is the ONLY file you edit and submit.
//
// Implements the supplied Vector<T> interface. See Vector.h for each
// function's contract; the supplied tests exercise ownership and capacities.
//
// Note: this file is #included at the bottom of Vector.h rather than compiled
// on its own (templates must be visible wherever they are used). Do not add
// it to a g++ command line; `make` already does the right thing.
// You may add your own non-member helper functions here.
// ============================================================================

#ifndef VECTOR_CPP
#define VECTOR_CPP
#include <algorithm>
#include "Vector.h"

// ---- Constructors and the Big 3 ----

// Default constructor. Creates an empty vector: size 0, capacity 0, no
// allocation.
template <typename T>
Vector<T>::Vector() : begin_(nullptr), end_(nullptr), end_cap_(nullptr){}



// Creates a vector of n elements, each value-initialized (0 for int, "" for
// std::string). Size and capacity are both n. Note that new T[n] does NOT
// zero ints. If n is 0, allocates nothing.
//
// Hint: you don't need a special case for each type. Assigning T() works for
// every type: it gives 0 for int, 0.0 for double, nullptr for pointers, ""
// for std::string, and calls the default constructor for a class type.
template <typename T>
Vector<T>::Vector(std::size_t n) : Vector(){
	//initilize with default contructors
	if(n != 0){
		begin_ = new T[n]{};
		end_ = end_cap_ = begin_ + n;
	}

}

// Creates a vector of n copies of value. Size and capacity are both n.
// If n is 0, allocates nothing.
template <typename T>
Vector<T>::Vector(std::size_t n, const T& value) : Vector(){

	if(n != 0){
		//pointer initialization
		begin_ = new T[n];
		end_ = end_cap_ = begin_ + n;
		for(std::size_t i = 0; i < n; i++){
			begin_[i] = value;
		}
	}
}

// Creates a vector holding the elements of il, in order: Vector<int> v{1, 2, 3}.
// Size and capacity are both il.size(). If il is empty, allocates nothing.
//
// Working with std::initializer_list<T>: when you write Vector<int> v{1, 2, 3},
// the compiler packs {1, 2, 3} into il. It is a read-only view of the values:
//   - il.size() is the number of values (3 here)
//   - you can loop over it:  for (const T& x : il) { ... }
//   - il.begin() and il.end() are pointers to const T, like your own begin()/end()
//   - it has NO operator[], so il[i] does not compile
// Reference: https://en.cppreference.com/w/cpp/utility/initializer_list
template <typename T>
Vector<T>::Vector(std::initializer_list<T> il) : Vector(){
	if(il.size() != 0){
		begin_ = new T[il.size()];
		end_ = end_cap_ = begin_ + il.size();


		std::size_t i = 0;
		for(const T& value : il){
			begin_[i] = value;
			i++;
		}
	}

}

// Copy constructor. Makes a deep copy of other. The new capacity is other.size()
// (not other.capacity()).
// If other is empty, allocates nothing.
template <typename T>
Vector<T>::Vector(const Vector& other) : Vector(){
	if(other.size() != 0){
		begin_ = new T[other.size()];
		end_ = end_cap_ = begin_ + other.size();

		for(std::size_t i = 0; i < other.size(); i++){
			begin_[i] = other.begin_[i];
		}
	}
}

// Copy assignment operator. Replaces this vector's contents with a deep copy
// of other's elements and frees the old buffer. The new capacity is
// other.size() (if 0, no buffer). Self-assignment (v = v) must leave v
// unchanged. Returns *this so that a = b = c works.
template <typename T>
Vector<T>& Vector<T>::operator=(const Vector& other) {
	if(this == &other){
		return *this;
	}

	if(other.size() < size()){
		resize(other.size());
	}
	reallocate(other.size());
	if(begin_ == nullptr) return *this;

	end_ = end_cap_;

	for(std::size_t i = 0; i < other.size(); i++){
		begin_[i] = other.begin_[i];
	}

	return *this;
}

// Destructor.
template <typename T>
Vector<T>::~Vector() {
    delete[] begin_;
}

// ---- Private helper ----

// Preserves the elements while replacing the buffer. Requires new_cap >= size().
// A zero capacity releases the buffer and resets all three pointers.
template <typename T>
void Vector<T>::reallocate(std::size_t new_cap) {
	std::size_t const vecSize = size();
	if(new_cap == 0){
		delete[] begin_;
		begin_ = end_ = end_cap_ = nullptr;
	}
	else{
		T* nBegin_ = new T[new_cap];
		for(size_t i = 0; i < vecSize; i++){
			nBegin_[i] = (begin_ == nullptr) ? T() : begin_[i];
		}
		if(begin_ != nullptr) delete[] begin_;
		begin_ = nBegin_;
		end_ = begin_ + vecSize;
		end_cap_ = begin_ + new_cap;
	}
}

// ---- Element access ----

// Returns a reference to element i. Do NOT check bounds: making sure
// i < size() is the caller's job, and an out-of-range i is undefined behavior
// (just like std::vector). at() is the version that checks.
template <typename T>
T& Vector<T>::operator[](std::size_t i) {
	return begin_[i];
}

// Same as above, for const vectors. operator== needs this: its parameters are
// const Vector&, so it can only read elements through a const member function.
template <typename T>
const T& Vector<T>::operator[](std::size_t i) const {
	return begin_[i];
}

// Returns a reference to element i.
// Throws std::out_of_range if i >= size().
template <typename T>
T& Vector<T>::at(std::size_t i) {
	if(i < size())
		return begin_[i];
	else
		throw std::out_of_range("Vector::an index is out of range");
}

// Returns a reference to the first element.
// Throws std::out_of_range if the vector is empty.
template <typename T>
T& Vector<T>::front() {
	if(size() == 0){
		throw::std::out_of_range("Vector::access attempt on empty vector");
	}

    return *begin_;
}

// Returns a reference to the last element.
// Throws std::out_of_range if the vector is empty.
template <typename T>
T& Vector<T>::back() {
	if(size() == 0){
		throw::std::out_of_range("Vector::access attempt on empty vector");
	}

	return end_[-1];
}

// Returns a pointer to the first element of the buffer, or nullptr if there
// is no buffer.
template <typename T>
T* Vector<T>::data() {
	return begin_;
}

// ---- Iterators ----

// Returns a pointer to the first element. Together with end(), this lets
// range-for loops and std::sort work on your Vector.
template <typename T>
T* Vector<T>::begin() {
	return begin_;
}

// Same as above, for const vectors. Together with end() const, this lets a
// range-for loop run over a const Vector&.
template <typename T>
const T* Vector<T>::begin() const {
    return begin_;
}

// Returns a pointer one past the last element (not past the end of the
// buffer). For an empty vector, end() == begin().
template <typename T>
T* Vector<T>::end() {
    return end_;

}

// Same as above, for const vectors.
template <typename T>
const T* Vector<T>::end() const {
	return end_;
}

// ---- Capacity ----

// Returns the number of elements currently stored.
template <typename T>
std::size_t Vector<T>::size() const {
	// Pointer subtraction requires an allocated array, even for an empty range.
	return begin_ == nullptr ? 0 : end_ - begin_;
}

// Returns how many elements the buffer can hold before it must grow.
template <typename T>
std::size_t Vector<T>::capacity() const {
	if(begin_ != nullptr)
		return end_cap_ - begin_;
	else
		return 0;
}

// Returns true if the vector has no elements (size() == 0).
template <typename T>
bool Vector<T>::empty() const {
	return size() == 0;
}

// Makes room for n elements up front, so that the vector can later grow to
// n elements without reallocating. Afterwards, capacity() is at least n (and
// exactly n if the buffer had to grow). The elements and size() are unchanged,
// and capacity never goes down.
template <typename T>
void Vector<T>::reserve(std::size_t n) {
	if(n > capacity()) reallocate(n);
}

// Shrinks the buffer so capacity() == size(), keeping the elements.
// Does nothing if they are already equal. If size() is 0, frees the buffer
// and all three pointers become nullptr.
template <typename T>
void Vector<T>::shrink_to_fit() {
	if(size() == 0 && begin_ != nullptr){
		delete[] begin_;
		begin_ = end_ = end_cap_ = nullptr;
	}

	else if(capacity() > size()){
		reallocate(size());
	}
}

// ---- Modifiers ----

// Appends a copy of value to the end. If the buffer is full, first grows it to
//     max(capacity() + 1, (size_t)(capacity() * growth_factor))
// Read growth_factor each time (the tests change it); do not hard-code 2.
// Careful: value may refer to an element of this vector (v.push_back(v[0])),
// which lives in the old buffer you are about to free.
template <typename T>
void Vector<T>::push_back(const T& value) {
	T tempValue = value;

	if(begin_ == nullptr){
		reallocate(1);
	}

	else if(size() == capacity()){
		reallocate(std::max(capacity() + 1, (std::size_t)(capacity() * growth_factor)));
	}

	*end_ = tempValue;
	++end_;
}

// Removes the last element. Capacity is unchanged and nothing is freed.
// Throws std::out_of_range if the vector is empty.
template <typename T>
void Vector<T>::pop_back() {
	if(size() == 0){
		throw std::out_of_range("Vector::tried to pop an empty vector");
	}
	--end_;
}

// Removes all elements: size becomes 0. Capacity is unchanged and nothing is
// freed.
template <typename T>
void Vector<T>::clear() {
    end_ = begin_;
}

// Changes the size to n.
//   - n < size(): removes elements from the end. Capacity is unchanged.
//   - n > size(): appends T() elements (0 for int). If n > capacity(), first
//     grows the buffer to exactly n. Slots reused from earlier elements must be
//     overwritten with T(), not left holding old values.
template <typename T>
void Vector<T>::resize(std::size_t n) {
	//edge case: if vector is not allocated and n == 0
	if(n > capacity()){
		reallocate(n);
	}
	if(n > size()){
		for(std::size_t i = 0; i < n - size(); i++){
			end_[i] = T();
		}
	}

	if(begin_ != nullptr) end_ = begin_ + n;
}

// Inserts a copy of value at position index, so that afterwards v[index] is
// the new value. The elements that were at index and after each move one
// position right. index == size() appends. If the buffer
// is full, first grows it with the same formula as push_back.
// Throws std::out_of_range if index > size() (and changes nothing).
// Like push_back, value may refer to an element of this vector.
template <typename T>
void Vector<T>::insert(std::size_t index, const T& value) {

	if(index > size()){
		throw std::out_of_range("Vector::insert: index not in range of vector");
	}

	T tempValue = value;

	std::size_t vecSize = size();
	if(size() == capacity()){
                reallocate(std::max(capacity() + 1,(std::size_t)( growth_factor * capacity())));
	}


	if(index == vecSize){
		push_back(tempValue);
		end_ = begin_ + vecSize + 1;
	}
	else{
		for(std::size_t i = vecSize; i > index; --i){
			begin_[i] = begin_[i-1];
		}

		begin_[index] = tempValue;
		end_ = begin_ + vecSize + 1;
	}
}


// Removes the element at index, shifting the elements after it one position
// left. Capacity is unchanged.
// Throws std::out_of_range if index >= size() (and changes nothing).
template <typename T>
void Vector<T>::erase(std::size_t index) {

        if(index >= size()){
                throw std::out_of_range("Vector::erase: index out of range");
        }

        if(index == size() -1){
                --end_;
        }
        else{

		for(std::size_t i = 0; i < size() - index -1; i++){
			begin_[index + i] = begin_[index + i + 1];}
                --end_;
        }
}

// ---- Non-member operators ----

// Returns true if a and b have the same size and every pair of elements
// compares equal with ==. Capacity does not matter.
template <typename T>
bool operator==(const Vector<T>& a, const Vector<T>& b) {
	if(a.size() != b.size()) return false;
	for(std::size_t i = 0; i < a.size(); i++){
		if(a[i] != b[i]) return false;
	}
	return true;
}

// Returns true if a and b are not equal (hint: use operator==).
template <typename T>
bool operator!=(const Vector<T>& a, const Vector<T>& b) {
	return !(a == b);

}

#endif
// ============================================================================
// CS32 P01 Part 1: Vector implementation
