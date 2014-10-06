//
//  valarray.h
//  Ant
//
//  Created by Anton Logunov on 6/28/14.
//  Copyright (c) 2014 Anton Logunov. All rights reserved.
//

#ifndef ARRAY_H
#define ARRAY_H

#include "ant.h"

namespace ant { namespace algebra {
    // user has no capability to push_back here
    
    // we won't give user to change capacity 
    // because it will make difficulties with the size
    template<class T>
    struct Array {
        Array() : Array(0) {}
        Array(Count size) 
            :   size_(size), 
                data_(new T[size]),
                capacity_(size) {}
        Array(const Array& array) 
            :   size_(array.size_), 
                capacity_(array.capacity_), 
                data_(new T(array.size_)) {
            
            std::copy_n(array.data_, size_, data_);
        }
        Array(Array&& array)
            :   size_(array.size_), 
                data_(array.data_), 
                capacity_(array.capacity_) {
            array.data_ = nullptr;
            array.size_ = 0;
            array.capacity_ = 0;
        }
        
        Array& operator=(const Array& array) {
            if (capacity_ < array.size_) {
                set_size(array.size_);
            } else {
                size_ = array.size_;
            }
            std::copy_n(array.data_, size_, data_);
            return *this;
        }
        Array& operator=(Array&& array) {
            delete [] data_;
            size_ = array.size_;
            data_ = array.data_;
            capacity_ = array.capacity_;
            array.size_ = 0;
            array.data_ = nullptr;
            array.capacity_ = 0;
            return *this;
        }
        
        Array& operator=(const T& g) {
            std::fill_n(data_, size_, g);
        }
        
        T& operator[](Int i) {
            return data_[i];
        } 
        const T& operator[](Int i) const {
            return data_[i];
        }
            
        // 100% copy
        void exact_resize(Count new_size) {
            T* new_data = new T[new_size];
            std::copy_n(data_, std::min(new_size, size_), new_data);
            delete [] data_;
            data_ = new_data;
            capacity_ = size_ = new_size;
        }
        // possible copy
        void resize(Count new_size) {
            if (new_size > capacity_) {
                exact_resize(new_size);
            } else {
                size_ = new_size;
            }
        }
        
        // 0% copy
        void set_exact_size(Count new_size) {
            delete data_;
            data_ = new T[new_size];
            size_ = capacity_ = new_size;
        }
        // no possible copy 
        void set_size(Count new_size) {
            if (new_size > capacity_) {
                set_exact_size(new_size);
            } else {
                size_ = new_size;
            }
        }
        
        Count size() const {
            return size_;
        }
        
        ~Array() {
            delete [] data_;
        }
        
    private:
        Count size_;
        T* data_;
        Count capacity_;
    };


} }

#endif
