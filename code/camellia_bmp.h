/* date = September 6th 2022 6:47 pm */

#ifndef CAMELLIA_BMP_H
#define CAMELLIA_BMP_H

#include "camellia_platform.h"

#define BITMAP_MAGIC_NUMBER 19778

typedef struct bmp_header {
    i32 Size;
    i32 AppID;
    i32 Offset;
} bmp_header;

typedef struct bitmap_info {
    i32 HeaderSize;
    i32 Width;
    i32 Height;
    i32 NumPlanes;
    i32 BitsPerPixel;
    i32 Compression;
    i32 ImageSize;
    i32 HorizontalResolution;
    i32 VerticalResolution;
    i32 ColorsUsed;
    i32 ColorsImportant;
} bitmap_info;

typedef struct bitmap_palette {
    u8 Blue;
    u8 Green;
    u8 Red;
    u8 Reserved;
} bitmap_palette;

typedef struct loaded_image {
    i32 Width;
    i32 Height;
    u8* Data;
} loaded_image;

void LoadBMP(const char* Path, loaded_image* Image);
void FreeBMP(loaded_image* Image);

#endif //CAMELLIA_BMP_H
