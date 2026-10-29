// ============================================================================
// CS32 P01 Part 1: Vector implementation
//
// This is the ONLY file you edit and submit.
//
// Every function below is a stub that throws "not implemented". Replace each
// stub body with your implementation. See Vector.h for what each function
// must do.
//
// Note: this file is #included at the bottom of Vector.h rather than compiled
// on its own (templates must be visible wherever they are used). Do not add
// it to a g++ command line; `make` already does the right thing.
// You may add your own non-member helper functions here.
// ============================================================================

#ifndef VECTOR_CPP
#define VECTOR_CPP

#include "Vector.h"

// ---- Constructors and the Big 3 ----

// Default constructor. Creates an empty vector: size 0, capacity 0, no
// allocation.
template <typename T>
Vector<T>::Vector() {
    // TODO
    throw std::logic_error("Vector() not implemented");
}

// Creates a vector of n elements, each value-initialized (0 for int, "" for
// std::string). Size and capacity are both n. Note that new T[n] does NOT
// zero ints. If n is 0, allocates nothing.
//
// Hint: you don't need a special case for each type. Assigning T() works for
// every type: it gives 0 for int, 0.0 for double, nullptr for pointers, ""
// for std::string, and calls the default constructor for a class type.
template <typename T>
Vector<T>::Vector(std::size_t n) {
    // TODO
    throw std::logic_error("Vector(n) not implemented");
}

// Creates a vector of n copies of value. Size and capacity are both n.
// If n is 0, allocates nothing.
template <typename T>
Vector<T>::Vector(std::size_t n, const T& value) {
    // TODO
    throw std::logic_error("Vector(n, value) not implemented");
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
Vector<T>::Vector(std::initializer_list<T> il) {
    // TODO
    throw std::logic_error("Vector(initializer_list) not implemented");
}

// Copy constructor. Makes a deep copy of other. The new capacity is other.size() 
// (not other.capacity()).
// If other is empty, allocates nothing.
template <typename T>
Vector<T>::Vector(const Vector& other) {
    // TODO
    throw std::logic_error("Vector(const Vector&) not implemented");
}

// Copy assignment operator. Replaces this vector's contents with a deep copy
// of other's elements and frees the old buffer. The new capacity is
// other.size() (if 0, no buffer). Self-assignment (v = v) must leave v
// unchanged. Returns *this so that a = b = c works.
template <typename T>
Vector<T>& Vector<T>::operator=(const Vector& other) {
    // TODO
    throw std::logic_error("operator= not implemented");
}

// Destructor. 
template <typename T>
Vector<T>::~Vector() {
    // TODO
}

// ---- Private helper ----

// Private helper: changes the capacity to exactly new_cap. Afterwards,
// capacity() == new_cap, size() and the elements are unchanged, and no memory
// is leaked. Any function that changes the capacity can be built on this one.
// Precondition: new_cap >= size(). If new_cap is 0, the vector ends up with no
// buffer at all.
template <typename T>
void Vector<T>::reallocate(std::size_t new_cap) {
    // TODO
    throw std::logic_error("reallocate() not implemented");
}

// ---- Element access ----

// Returns a reference to element i. Do NOT check bounds: making sure
// i < size() is the caller's job, and an out-of-range i is undefined behavior
// (just like std::vector). at() is the version that checks.
template <typename T>
T& Vector<T>::operator[](std::size_t i) {
    // TODO
    throw std::logic_error("operator[] not implemented");
}

// Same as above, for const vectors. operator== needs this: its parameters are
// const Vector&, so it can only read elements through a const member function.
template <typename T>
const T& Vector<T>::operator[](std::size_t i) const {
    // TODO
    throw std::logic_error("operator[] const not implemented");
}

// Returns a reference to element i.
// Throws std::out_of_range if i >= size().
template <typename T>
T& Vector<T>::at(std::size_t i) {
    // TODO
    throw std::logic_error("at() not implemented");
}

// Returns a reference to the first element.
// Throws std::out_of_range if the vector is empty.
template <typename T>
T& Vector<T>::front() {
    // TODO
    throw std::logic_error("front() not implemented");
}

// Returns a reference to the last element.
// Throws std::out_of_range if the vector is empty.
template <typename T>
T& Vector<T>::back() {
    // TODO
    throw std::logic_error("back() not implemented");
}

// Returns a pointer to the first element of the buffer, or nullptr if there
// is no buffer.
template <typename T>
T* Vector<T>::data() {
    // TODO
    throw std::logic_error("data() not implemented");
}

// ---- Iterators ----

// Returns a pointer to the first element. Together with end(), this lets
// range-for loops and std::sort work on your Vector.
template <typename T>
T* Vector<T>::begin() {
    // TODO
    throw std::logic_error("begin() not implemented");
}

// Same as above, for const vectors. Together with end() const, this lets a
// range-for loop run over a const Vector&.
template <typename T>
const T* Vector<T>::begin() const {
    // TODO
    throw std::logic_error("begin() const not implemented");
}

// Returns a pointer one past the last element (not past the end of the
// buffer). For an empty vector, end() == begin().
template <typename T>
T* Vector<T>::end() {
    // TODO
    throw std::logic_error("end() not implemented");
}

// Same as above, for const vectors.
template <typename T>
const T* Vector<T>::end() const {
    // TODO
    throw std::logic_error("end() const not implemented");
}

// ---- Capacity ----

// Returns the number of elements currently stored.
template <typename T>
std::size_t Vector<T>::size() const {
    // TODO
    throw std::logic_error("size() not implemented");
}

// Returns how many elements the buffer can hold before it must grow.
template <typename T>
std::size_t Vector<T>::capacity() const {
    // TODO
    throw std::logic_error("capacity() not implemented");
}

// Returns true if the vector has no elements (size() == 0).
template <typename T>
bool Vector<T>::empty() const {
    // TODO
    throw std::logic_error("empty() not implemented");
}

// Makes room for n elements up front, so that the vector can later grow to
// n elements without reallocating. Afterwards, capacity() is at least n (and
// exactly n if the buffer had to grow). The elements and size() are unchanged,
// and capacity never goes down.
template <typename T>
void Vector<T>::reserve(std::size_t n) {
    // TODO
    throw std::logic_error("reserve() not implemented");
}

// Shrinks the buffer so capacity() == size(), keeping the elements.
// Does nothing if they are already equal. If size() is 0, frees the buffer
// and all three pointers become nullptr.
template <typename T>
void Vector<T>::shrink_to_fit() {
    // TODO
    throw std::logic_error("shrink_to_fit() not implemented");
}

// ---- Modifiers ----

// Appends a copy of value to the end. If the buffer is full, first grows it to
//     max(capacity() + 1, (size_t)(capacity() * growth_factor))
// Read growth_factor each time (the tests change it); do not hard-code 2.
// Careful: value may refer to an element of this vector (v.push_back(v[0])),
// which lives in the old buffer you are about to free.
template <typename T>
void Vector<T>::push_back(const T& value) {
    // TODO
    throw std::logic_error("push_back() not implemented");
}

// Removes the last element. Capacity is unchanged and nothing is freed.
// Throws std::out_of_range if the vector is empty.
template <typename T>
void Vector<T>::pop_back() {
    // TODO
    throw std::logic_error("pop_back() not implemented");
}

// Removes all elements: size becomes 0. Capacity is unchanged and nothing is
// freed.
template <typename T>
void Vector<T>::clear() {
    // TODO
    throw std::logic_error("clear() not implemented");
}

// Changes the size to n.
//   - n < size(): removes elements from the end. Capacity is unchanged.
//   - n > size(): appends T() elements (0 for int). If n > capacity(), first
//     grows the buffer to exactly n. Slots reused from earlier elements must be
//     overwritten with T(), not left holding old values.
template <typename T>
void Vector<T>::resize(std::size_t n) {
    // TODO
    throw std::logic_error("resize() not implemented");
}

// Inserts a copy of value at position index, so that afterwards v[index] is
// the new value. The elements that were at index and after each move one
// position right. index == size() appends. If the buffer
// is full, first grows it with the same formula as push_back.
// Throws std::out_of_range if index > size() (and changes nothing).
// Like push_back, value may refer to an element of this vector.
template <typename T>
void Vector<T>::insert(std::size_t index, const T& value) {
    // TODO
    throw std::logic_error("insert() not implemented");
}

// Removes the element at index, shifting the elements after it one position
// left. Capacity is unchanged.
// Throws std::out_of_range if index >= size() (and changes nothing).
template <typename T>
void Vector<T>::erase(std::size_t index) {
    // TODO
    throw std::logic_error("erase() not implemented");
}

// ---- Non-member operators ----

// Returns true if a and b have the same size and every pair of elements
// compares equal with ==. Capacity does not matter.
template <typename T>
bool operator==(const Vector<T>& a, const Vector<T>& b) {
    // TODO
    throw std::logic_error("operator== not implemented");
}

// Returns true if a and b are not equal (hint: use operator==).
template <typename T>
bool operator!=(const Vector<T>& a, const Vector<T>& b) {
    // TODO
    throw std::logic_error("operator!= not implemented");
}

#endif
