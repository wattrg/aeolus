#ifndef __ARRAY_H_
#define __ARRAY_H_

#include <iterator>
#include <cstddef>
#include <stddef.h>
#include <assert.h>

template <typename T>
class Array {
public:
    Array() {}
    Array(size_t capacity) : _capacity(capacity) {
        _data = new T[capacity]; 
    }

    ~Array(){
        if (_data) delete _data;
    }

    void push_back(T value){
        if (_size < _capacity) {
            _data[_size] = value;
            _size ++;
        }
        else {
            throw std::runtime_error("The array is full you silly sausage!");
        }

    }

    size_t size() const {return _size;}
    T& operator[] (size_t index) { 
        assert (index > 0 && index < _capacity && "Index out of range");
        return _data[index]; 
    }

    struct Iterator {
        using iterator_category = std::forward_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = T;
        using pointer = T*;
        using reference = T&;

        Iterator(pointer ptr) : _ptr(ptr) {}

        reference operator*() const {return *_ptr;}
        pointer operator->() {return _ptr;}

        Iterator& operator++(){_ptr++; return *this;}
        Iterator& operator++(int) {Iterator tmp = *this; ++(*this); return tmp; }

        friend bool operator == (const Iterator& a, const Iterator& b) {
            return a._ptr == b._ptr; 
        };
        friend bool operator != (const Iterator& a, const Iterator& b) {
            return a._ptr != b._ptr; 
        };

    private:
        pointer _ptr;
    };

    Iterator begin() const { return Iterator(&_data[0]); }
    Iterator end() const { return Iterator(&_data[_size]); }

private:
    size_t _size = 0;
    size_t _capacity = 0;
    T * _data = nullptr;
};


#endif
