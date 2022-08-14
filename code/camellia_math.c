#include "camellia_math.h"

f32 Normalize(f32 Input, f32 Min, f32 Max)
{
    f32 Average = (Min + Max) / 2;
    f32 Range = (Max - Min) / 2;
    return (Input - Average) / Range;
}