/* date = August 11th 2022 9:15 pm */

#ifndef CAMELLIA_PLATFORM_H
#define CAMELLIA_PLATFORM_H

#include <stdint.h>

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int32_t bool32;
typedef float f32;
typedef double f64;

#define ArrayCount(arr) (sizeof((arr)) / (sizeof((arr)[0])))
#define Kilobytes(number) ((number)*1024ull)
#define Megabytes(number) (Kilobytes(number) * 1024ull)
#define Gigabytes(number) (Megabytes(number) * 1024ull)
#define Terabytes(number) (Gigabytes(number) * 1024ull)
#define Minimum(a, b) ((a) < (b) ? (a) : (b))
#define Maximum(a, b) ((a) > (b) ? (a) : (b))
#define Align16(val) ((val + 15) & ~15)
#define Align4(val) ((val + 3) & ~3)

#ifdef GAME_DEBUG
#define Assert(expression) if (!(expression)) { *(int*)0 = 0; }
#else
#define Assert(expression)
#endif

#define internal static
#define global static
#define local_persist static
#define false 0
#define true 1

#define MAX_GAMEPADS 4

typedef struct platform_gamepad {
    // NOTE(milo): State
    bool32 Connected;
    
    // NOTE(milo): Buttons
    bool32 A;
    bool32 B;
    bool32 Y;
    bool32 X;
    bool32 Start;
    bool32 Select;
    bool32 LeftShoulder;
    bool32 RightShoulder;
    bool32 LeftThumb;
    bool32 RightThumb;
    bool32 DpadLeft;
    bool32 DpadRight;
    bool32 DpadUp;
    bool32 DpadDown;
    
    // NOTE(milo): Analog triggers
    f32 LeftTrigger;
    f32 RightTrigger;
    
    // NOTE(milo): Joysticks
    // TODO(milo): Use vectors
    f32 LeftX;
    f32 LeftY;
    f32 RightX;
    f32 RightY;
    
    // NOTE(milo): Vibration
    f32 LeftMotor;
    f32 RightMotor;
} platform_gamepad;

typedef struct platform_state {
    platform_gamepad Gamepads[MAX_GAMEPADS];
} platform_state;

extern platform_state PlatformState;

#endif // CAMELLIA_PLATFORM_H
