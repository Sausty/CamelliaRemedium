#include "camellia_win32.h"
#include "camellia_math.h"
#include "camellia_renderer_d3d12.h"

#include <string.h>
#include <xinput.h>

#define XINPUT_GET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_STATE *pState)
typedef XINPUT_GET_STATE(xinput_get_state);
#define XINPUT_SET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_VIBRATION *pVibration)
typedef XINPUT_SET_STATE(xinput_set_state);

XINPUT_GET_STATE(XInputGetStateStub) { return ERROR_DEVICE_NOT_CONNECTED; }
global xinput_get_state *xInputGetState_ = XInputGetStateStub;
XINPUT_SET_STATE(XInputSetStateStub) { return ERROR_DEVICE_NOT_CONNECTED; }
global xinput_set_state *xInputSetState_ = XInputSetStateStub;

#define XInputGetState xInputGetState_
#define XInputSetState xInputSetState_

global win32_state Platform;
platform_state PlatformState;

internal
void Win32GetExePath()
{
    DWORD FileNameSize = GetModuleFileNameA(NULL, Platform.ExecutablePath, sizeof(Platform.ExecutablePath));
    char *OnePathLastSlash = Platform.ExecutablePath;
    for (char *Scan = Platform.ExecutablePath + FileNameSize;; --Scan)
    {
        if (*Scan == '\\')
        {
            OnePathLastSlash = Scan + 1;
            break;
        }
    }
    Platform.ExecutableDirLength = (i32)(OnePathLastSlash - Platform.ExecutablePath);
}

internal
void Win32BuildPathInExeDir(char* Source, i32 SourceSize, char* Dest, i32 DestSize)
{
    strncpy_s(Dest, DestSize, Platform.ExecutablePath, Platform.ExecutableDirLength);
    strncpy_s(Dest + Platform.ExecutableDirLength, DestSize - Platform.ExecutableDirLength, Source, SourceSize);
}

internal
FILETIME Win32GetFileLastWriteTime(char* FileName)
{
    FILETIME Result = {0};
    WIN32_FILE_ATTRIBUTE_DATA Data;
    
    if (GetFileAttributesExA(FileName, GetFileExInfoStandard, &Data)) {
        Result = Data.ftLastWriteTime;
    }
    
    return(Result);
}

internal
win32_window_dimension Win32GetWindowDimension(HWND Window)
{
    win32_window_dimension Result = {0};
    RECT ClientRect;
    GetClientRect(Window, &ClientRect);
    Result.Width = ClientRect.right - ClientRect.left;
    Result.Height = ClientRect.bottom - ClientRect.top;
    return(Result);
}

internal
void Win32LoadXInput()
{
    HMODULE Library = LoadLibraryA("xinput1_4.dll");
    if (Library)
    {
        XInputGetState = (xinput_get_state*)GetProcAddress(Library, "XInputGetState");
        XInputSetState = (xinput_set_state*)GetProcAddress(Library, "XInputSetState");
        
        Assert(XInputGetState && XInputSetState);
    }
}

internal
win32_game_code Win32LoadGameCode(char* DLLPath, char* DLLTempPath, char* LockPath) 
{
    win32_game_code Result = {0};
    
    WIN32_FILE_ATTRIBUTE_DATA Ignored;
    if (!GetFileAttributesExA(LockPath, GetFileExInfoStandard, &Ignored)) 
    {
        Result.GameDLLLastWriteTime = Win32GetFileLastWriteTime(DLLPath);
        
        while (true) 
        {
            if (CopyFile(DLLPath, DLLTempPath, FALSE)) 
            {
                break;
            }
            
            if (GetLastError() == ERROR_FILE_NOT_FOUND)
            {
                break;
            }
        }
        
        Result.GameCodeDLL = LoadLibraryA(DLLTempPath);
        if (Result.GameCodeDLL)
        {
            Result.Update = (game_update*)GetProcAddress(Result.GameCodeDLL, "GameUpdate");
            
            Result.IsValid = (!!Result.Update);
        }
        
        if (!Result.IsValid)
        {
            Result.Update = GameUpdateStub;
        }
    }
    
    return(Result);
}

internal void
Win32UnloadGameCode(win32_game_code *GameCode)
{
    if (GameCode->GameCodeDLL)
    {
        FreeLibrary(GameCode->GameCodeDLL);
        GameCode->GameCodeDLL = 0;
    }
    
    GameCode->IsValid = false;
    GameCode->Update = GameUpdateStub;
}

internal
f32 Win32ApplyDeadzone(f32 Value, f32 MaxValue, f32 Deadzone)
{
    if (Value < -Deadzone)
    {
        Value += Deadzone;
    }
    else if (Value > Deadzone)
    {
        Value -= Deadzone;
    }
    else
    {
        return 0.0f;
    }
    f32 NormValue = (f32)Value / (f32)(MaxValue - Deadzone);
    return Maximum(-1.0f, Minimum(NormValue, 1.0f));
}

internal
void Win32UpdateXInput()
{
    for (i32 ControllerIndex = 0; ControllerIndex < MAX_GAMEPADS; ControllerIndex++)
    {
        XINPUT_STATE State;
        DWORD Result = XInputGetState(ControllerIndex, &State);
        
        bool32 Connected = (Result == ERROR_SUCCESS);
        PlatformState.Gamepads[ControllerIndex].Connected = Connected;
        
        if (Connected)
        {
            XINPUT_GAMEPAD Pad = State.Gamepad;
            
            PlatformState.Gamepads[ControllerIndex].A = (Pad.wButtons & XINPUT_GAMEPAD_A) == true;
            PlatformState.Gamepads[ControllerIndex].B = (Pad.wButtons & XINPUT_GAMEPAD_B) == true;
            PlatformState.Gamepads[ControllerIndex].Y = (Pad.wButtons & XINPUT_GAMEPAD_Y) == true;
            PlatformState.Gamepads[ControllerIndex].X = (Pad.wButtons & XINPUT_GAMEPAD_X) == true;
            PlatformState.Gamepads[ControllerIndex].Start = (Pad.wButtons & XINPUT_GAMEPAD_START) == true;
            PlatformState.Gamepads[ControllerIndex].Select = (Pad.wButtons & XINPUT_GAMEPAD_BACK) == true;
            PlatformState.Gamepads[ControllerIndex].LeftShoulder = (Pad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) == true;
            PlatformState.Gamepads[ControllerIndex].RightShoulder = (Pad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) == true;
            PlatformState.Gamepads[ControllerIndex].LeftThumb = (Pad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB) == true;
            PlatformState.Gamepads[ControllerIndex].RightThumb = (Pad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) == true;
            PlatformState.Gamepads[ControllerIndex].DpadLeft = (Pad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) == true;
            PlatformState.Gamepads[ControllerIndex].DpadRight = (Pad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) == true;
            PlatformState.Gamepads[ControllerIndex].DpadUp = (Pad.wButtons & XINPUT_GAMEPAD_DPAD_UP) == true;
            PlatformState.Gamepads[ControllerIndex].DpadDown = (Pad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) == true;
            
            PlatformState.Gamepads[ControllerIndex].LeftTrigger = (f32)Pad.bLeftTrigger / 255;
            PlatformState.Gamepads[ControllerIndex].RightTrigger = (f32)Pad.bRightTrigger / 255;
            
            f32 NormLX = Normalize((f32)Pad.sThumbLX, -32767, 32767);
            f32 NormLY = Normalize((f32)Pad.sThumbLY, -32767, 32767);
            f32 NormRX = Normalize((f32)Pad.sThumbRX, -32767, 32767);
            f32 NormRY = Normalize((f32)Pad.sThumbRY, -32767, 32767);
            
            f32 LX = Win32ApplyDeadzone(NormLX,  1.0f, Normalize(XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE, SHRT_MIN, SHRT_MAX));
            f32 LY = Win32ApplyDeadzone(NormLY,  1.0f, Normalize(XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE, SHRT_MIN, SHRT_MAX));
            f32 RX = Win32ApplyDeadzone(NormRX,  1.0f, Normalize(XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE, SHRT_MIN, SHRT_MAX));
            f32 RY = Win32ApplyDeadzone(NormRY,  1.0f, Normalize(XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE, SHRT_MIN, SHRT_MAX));
            
            PlatformState.Gamepads[ControllerIndex].LeftX = LX;
            PlatformState.Gamepads[ControllerIndex].LeftY = LY;
            PlatformState.Gamepads[ControllerIndex].RightX = RX;
            PlatformState.Gamepads[ControllerIndex].RightY = RY;
            
            XINPUT_VIBRATION Vibration;
            Vibration.wRightMotorSpeed = (u16)(PlatformState.Gamepads[ControllerIndex].RightMotor * 65335.0f);
            Vibration.wLeftMotorSpeed = (u16)(PlatformState.Gamepads[ControllerIndex].LeftMotor * 65335.0f);
        }
    }
}

LRESULT CALLBACK
Win32MainWindowCallback(HWND Window, 
                        UINT Message, 
                        WPARAM WParam, 
                        LPARAM LParam)
{
    LRESULT Result = 0;
    
    switch (Message)
    {
        case WM_SIZE:
        {
            OutputDebugStringA("WM_SIZE\n");
        } break;
        
        case WM_CREATE:
        {
            OutputDebugString("WM_CREATE\n");
            Platform.Running = true;
        } break;
        
        case WM_CLOSE:
        {
            Platform.Running = false;
            OutputDebugStringA("WM_CLOSE\n");
        } break;	
        
        default: 
        {
            Result = DefWindowProc(Window, Message, WParam, LParam);
        } break;
    }
    
    return(Result);
}

int CALLBACK WinMain(HINSTANCE hInstance, 
                     HINSTANCE hPrevInstance, 
                     LPSTR lpCmdLine, 
                     int nCmdShow)
{
    WNDCLASS WindowClass = {0};
    WindowClass.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
    WindowClass.hInstance = hInstance;
    WindowClass.lpfnWndProc = Win32MainWindowCallback;
    WindowClass.lpszClassName = "CamelliaRemediumWindowClass";
    
    if (!RegisterClass(&WindowClass))
    {
        // TODO(milo): Error loading
    }
    
    HWND WindowHandle = CreateWindowEx(0,
                                       "CamelliaRemediumWindowClass",
                                       "Camellia Remedium",
                                       WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                                       CW_USEDEFAULT,
                                       CW_USEDEFAULT,
                                       CW_USEDEFAULT,
                                       CW_USEDEFAULT,
                                       0,
                                       0,
                                       hInstance,
                                       0);
    if (!WindowHandle)
    {
        // TODO(milo): Error loading
    }
    
    RendererInit(WindowHandle);
    
    Win32GetExePath();
    
    char GameDLLName[] = "camellia.dll";
    char GameDLLPath[WIN32_MAX_PATH] = {0};
    Win32BuildPathInExeDir(GameDLLName, sizeof(GameDLLName) - 1, GameDLLPath, sizeof(GameDLLPath));
    
    char GameTempDLLName[] = "camellia1.dll";
    char GameTempDLLPath[WIN32_MAX_PATH] = {0};
    Win32BuildPathInExeDir(GameTempDLLName, sizeof(GameTempDLLName) - 1, GameTempDLLPath, sizeof(GameTempDLLPath));
    
    char GameLockFileName[] = "camellia_lock.tmp";
    char GameLockFilePath[WIN32_MAX_PATH] = {0};
    Win32BuildPathInExeDir(GameLockFileName, sizeof(GameLockFileName) - 1, GameLockFilePath, sizeof(GameLockFilePath));
    
    Win32LoadXInput();
    win32_game_code GameCode = Win32LoadGameCode(GameDLLPath, GameTempDLLPath, GameLockFilePath);
    while (Platform.Running)
    {
        FILETIME NewDLLWriteTime = Win32GetFileLastWriteTime(GameDLLPath);
        if (CompareFileTime(&NewDLLWriteTime, &GameCode.GameDLLLastWriteTime) != 0)
        {
            Win32UnloadGameCode(&GameCode);
            GameCode = Win32LoadGameCode(GameDLLPath, GameTempDLLPath, GameLockFilePath);
        }
        
        Win32UpdateXInput();
        
        if (GameCode.Update)
        {
            GameCode.Update();
        }
        
        MSG Message;
        while (PeekMessageA(&Message, WindowHandle, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&Message);
            DispatchMessage(&Message);
        }
    }
    Win32UnloadGameCode(&GameCode);
    
    RendererExit();
    
    return(0);
}