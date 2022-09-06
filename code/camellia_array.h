/* date = September 5th 2022 8:11 pm */

#ifndef CAMELLIA_ARRAY_H
#define CAMELLIA_ARRAY_H

#include "camellia_platform.h"

#define ARRAY_DEFAULT_CAPACITY 1
#define ARRAY_RESIZE_FACTOR 2

enum {
    Array_Capacity,
    Array_Length,
    Array_Stride,
    Array_FieldLength
};

void* _ArrayCreate(u64 Length, u64 Stride);
void _ArrayDestroy(void* Array);
u64 _ArrayFieldGet(void* Array, u64 Field);
void _ArrayFieldSet(void* Array, u64 Field, u64 Value);
void* _ArrayResize(void* Array);
void* _ArrayPush(void* Array, const void* Value);
void _ArrayPop(void* Array, void* Destination);

#define ArrayCreate(type) _ArrayCreate(ARRAY_DEFAULT_CAPACITY, sizeof(type))
#define ArrayReserve(type, capacity) _ArrayCreate(capacity, sizeof(type))
#define ArrayDestroy(array) _ArrayDestroy(array)
#define ArrayPush(array, value) _ArrayPush(array, &value)
#define ArrayPop(array, dest) _ArrayPop(array, dest)
#define ArrayClear(array) _ArrayFieldSet(array, Array_Length, 0)
#define ArrayCapacity(array) _ArrayFieldGet(array, Array_Capacity)
#define ArrayLength(array) _ArrayFieldGet(array, Array_Length)
#define ArrayStride(array) _ArrayFieldGet(array, Array_Stride)
#define ArrayLengthSet(array, value) _ArrayFieldSet(array, Array_Length, value)

#endif //CAMELLIA_ARRAY_H