@echo off

REM Set all the variables
set CommonCompilerFlags=-MTd -nologo -fp:fast -Gm- -GR- -EHa- -Od -Oi -WX -W4 -wd4201 -wd4100 -wd4189 -wd4505 -wd4244 -wd4456 -wd4047 -DGAME_DEBUG=1 -D_CRT_SECURE_NO_WARNINGS -FC -Z7
set CommonLinkerFlags= -incremental:no -opt:ref user32.lib dxguid.lib

REM Create build directory if it doesn't exist.
IF NOT EXIST build (
    MKDIR build
)

REM Build the project
PUSHD build
del *.pdb > NUL 2> NUL
cl %CommonCompilerFlags% ../code/camellia.c ../code/camellia_win32.c ../code/camellia_math.c ../code/camellia_renderer_d3d12.c ../code/camellia_dsound.c ../code/camellia_wav.c ../code/camellia_renderer.c -Fmcamelia_win32.map /link %CommonLinkerFlags%
POPD

echo -- [GAME BUILD] Build completed. --