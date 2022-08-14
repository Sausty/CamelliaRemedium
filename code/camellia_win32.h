/* date = August 12th 2022 4:24 pm */

#ifndef CAMELLIA_WIN32_H
#define CAMELLIA_WIN32_H

#include <Windows.h>

#include "camellia_platform.h"
#include "camellia.h"

#define WIN32_MAX_PATH MAX_PATH

typedef struct win32_state {
    bool32 Running;
    
    char ExecutablePath[WIN32_MAX_PATH];
    i32 ExecutableDirLength;
} win32_state;

typedef struct win32_window_dimension {
    u32 Width;
    u32 Height;
} win32_window_dimension;

typedef struct win32_game_code {
    HMODULE GameCodeDLL;
    FILETIME GameDLLLastWriteTime;
    bool32 IsValid;
    
    game_update *Update;
} win32_game_code;


#endif //CAMELLIA_WIN32_H
