//
// Created by ivan on 11.4.21.
//

#ifndef RELOAD_ARRAY_H
#define RELOAD_ARRAY_H

#include "Common.h"

#include <array>

//template <class T, uint32_t N>
//class Array {
//public:
//    [[nodiscard]]int Count() const { return N; }
//    [[nodiscard]]int ByteSize() const { return sizeof(data); }
//    void Zero() { memset(data, 0, sizeof(data) ); }
//    void Memset(const char fill) { memset(data, fill, N * sizeof(*data)); }
//
//    const T &   operator[](int index) const {
//        assert(static_cast<uint32_t>(index) < N);
//        return data[index];
//    }
//
//    T &	operator[](int index) {
//        assert(static_cast<uint32_t>(index) < N);
//        return data[index];
//    }
//
//    const T * Data() const { return data; }
//    T * Data() { return data; }
//private:
//    T data[N];
//};

template <class T, uint32_t N>
struct Array : std::array<T, N> {
public:
    constexpr Array() noexcept : std::array<T, N>{} {}
    void set(T value) { std::fill(this->begin(), this->end(), value); }
};

template<class T, uint32_t N1, uint32_t N2>
struct Array2D {
    typedef Array<Array<T, N2>, N1> arr;
};

#endif //RELOAD_ARRAY_H
