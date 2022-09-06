#include "camellia_array.h"

#include <stdlib.h>

void* _ArrayCreate(u64 Length, u64 Stride)
{
    u64 HeaderSize = Array_FieldLength * sizeof(u64);
    u64 ArraySize = Length * Stride;
    u64* NewArray = PlatformState.HeapAlloc(HeaderSize + ArraySize);
    NewArray[Array_Capacity] = Length;
    NewArray[Array_Length] = 0;
    NewArray[Array_Stride] = Stride;
    return (void*)(NewArray + Array_FieldLength);
}

void _ArrayDestroy(void* Array)
{
	u64* Header = (u64*)Array - Array_FieldLength;
    PlatformState.HeapFree(Header);
}

u64 _ArrayFieldGet(void* Array, u64 Field)
{
    u64* Header = (u64*)Array - Array_FieldLength;
    return Header[Field];
}

void _ArrayFieldSet(void* Array, u64 Field, u64 Value)
{
    u64* Header = (u64*)Array - Array_FieldLength;
    Header[Field] = Value;
}

void* _ArrayResize(void* Array)
{
	u64 Length = ArrayLength(Array);
    u64 Stride = ArrayStride(Array);
    void* Temp = _ArrayCreate((ARRAY_RESIZE_FACTOR * ArrayCapacity(Array)), Stride);
    memcpy(Temp, Array, Length * Stride);
    
    _ArrayFieldSet(Temp, Array_Length, Length);
    _ArrayDestroy(Array);
    return Temp;
}

void* _ArrayPush(void* Array, const void* Value)
{
	u64 Length = ArrayLength(Array);
    u64 Stride = ArrayStride(Array);
    if (Length >= ArrayCapacity(Array)) 
    {
        Array = _ArrayResize(Array);
    }
    
    u64 Address = (u64)Array;
    Address += (Length * Stride);
    memcpy((void*)Address, Value, Stride);
    _ArrayFieldSet(Array, Array_Length, Length + 1);
    return Array;
}

void _ArrayPop(void* Array, void* Destination)
{
	u64 Length = ArrayLength(Array);
    u64 Stride = ArrayStride(Array);
    u64 Address = (u64)Array;
    Address += ((Length - 1) * Stride);
    memcpy(Destination, (void*)Address, Stride);
    _ArrayFieldSet(Array, Array_Length, Length - 1);
}