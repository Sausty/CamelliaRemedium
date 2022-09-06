#include "camellia_bmp.h"

#include <stdio.h>

internal
void ReadRLE8(FILE* File, u8* Dest, bitmap_palette* Palette, i32 Width, i32 Height)
{
    u8 Byte, Index, *P;
    u8 XOffset, YOffset = 0;
    u8 KeepReading = 1;
    i32 CurrentLine = 0;
    
    P = Dest;
    while (KeepReading)
    {
        Byte = fgetc(File);
        if (Byte)
        {
            Index = fgetc(File);
            for (i32 I = 0; I < Byte; I++)
            {
                *P = Palette[Index].Red;
                P++;
                *P = Palette[Index].Green;
                P++;
                *P = Palette[Index].Blue;
                P++;
                *P = Palette[Index].Reserved;
                P++;
                *P = 0xFF;
                P++;
            }
        }
        else
        {
            Byte = fgetc(File);
            switch (Byte)
            {
                case 0:
                {
                    CurrentLine++;
                    P = Dest + CurrentLine * Width * 4;
                } break;
                
                case 1:
                {
                    KeepReading = 0;
                } break;
                
                case 2:
                {
                    XOffset = fgetc(File);
                    YOffset = fgetc(File);
                    CurrentLine += YOffset;
                    P += YOffset * Width * 4 + XOffset * 4;
                } break;
                
                default:
                {
                    for (i32 I = 0; I < Byte; I++)
                    {
                        Index = fgetc(File);
                        *P = Palette[Index].Red;
                        P++;
                        *P = Palette[Index].Green;
                        P++;
                        *P = Palette[Index].Blue;
                        P++;
                        *P = Palette[Index].Reserved;
                        P++;
                        *P = 0xFF;
                        P++;
                    }
                    
                    if (Byte % 2)
                    {
                        (void)fseek(File, 1, SEEK_CUR);
                    }
                } break;
            }
        }
        
        if (P >= Dest + Width * Height * 4) KeepReading = 0;
    }
}

internal
void ReadRLE4(FILE* File, u8* Dest, bitmap_palette* Palette, i32 Width, i32 Height)
{
    u8 Byte1, Byte2, Index1, Index2, *P;
    u8 XOffset, YOffset;
    u8 Bitmask = 0x0F;
    u8 KeepReading = 1;
    i32 CurrentLine = 0;
    
    P = Dest;
    while (KeepReading)
    {
        Byte1 = fgetc(File);
        if (Byte1)
        {
            Byte2 = fgetc(File);
            Index1 = Byte2 >> 4;
            Index2 = Byte2 & Bitmask;
            for (i32 I = 0; I < (Byte1 / 2); I++)
            {
                *P = Palette[Index1].Red; 
                P++;
				*P = Palette[Index1].Green; 
                P++;
				*P = Palette[Index1].Blue; 
                P++;
				*P = 0x0F; 
                P++;
                
				*P = Palette[Index2].Red; 
                P++;
				*P = Palette[Index2].Green; 
                P++;
				*P = Palette[Index2].Blue; 
                P++;
				*P = 0x0F; 
                P++;
            }
            if (Byte1 % 2) 
            {
                *P = Palette[Index1].Red; 
                P++;
				*P = Palette[Index1].Green; 
                P++;
				*P = Palette[Index1].Blue; 
                P++;
				*P = 0x0F; 
                P++;
            }
        }
        else
        {
            Byte2 = fgetc(File);
            switch (Byte2)
            {
                case 0:
                {
                    CurrentLine++;
                    P = Dest + CurrentLine * Width * 4;
                } break;
                
                case 1:
                {
                    KeepReading = 0;
                } break;
                
                case 2:
                {
                    XOffset = fgetc(File);
                    YOffset = fgetc(File);
                    CurrentLine += YOffset;
                    P += YOffset * Width * 4 + XOffset * 4;
                } break;
                
                default:
                {
                    for (i32 I = 0; I < (Byte2 / 2); I++)
                    {
                        Byte1 = fgetc(File);
                        Index1 = Byte1 >> 4;
                        
                        *P = Palette[Index1].Red; 
                        P++;
                        *P = Palette[Index1].Green; 
                        P++;
                        *P = Palette[Index1].Blue; 
                        P++;
                        *P = 0x0F; 
                        P++;
                        
                        Index2 = Byte1 & Bitmask;
                        *P = Palette[Index2].Red; 
                        P++;
                        *P = Palette[Index2].Green; 
                        P++;
                        *P = Palette[Index2].Blue; 
                        P++;
                        *P = 0x0F; 
                        P++;
                    }
                    if (Byte2 % 2)
                    {
                        Byte1 = fgetc(File);
                        Index1 = Byte1 >> 4;
                        *P = Palette[Index1].Red; 
                        P++;
                        *P = Palette[Index1].Green; 
                        P++;
                        *P = Palette[Index1].Blue; 
                        P++;
                        *P = 0x0F; 
                        P++;
                    }
                    if (((Byte2 + 1) / 2) % 2)
                        fseek(File, 1, SEEK_CUR);
                } break;
            }
        }
    }
}

internal
void Read32(FILE* File, u8* Dest, bitmap_palette* Palette, i32 Width, i32 Height)
{
    u8 Pixel[4], *P;
    P = Dest;
    
    for (i32 Y = 0; Y < Height; Y++)
    {
        for (i32 X = 0; X < Width; X++)
        {
            fread(Pixel, 4, 1, File);
            
            *P = Pixel[3];
            P++;
            *P = Pixel[2];
            P++;
            *P = Pixel[1];
            P++;
            *P = 0xFF;
            P++;
        }
    }
}

internal
void Read24(FILE* File, u8* Dest, bitmap_palette* Palette, i32 Width, i32 Height)
{
    u8 Pixel[3], *P;
    P = Dest;
    
    for (i32 Y = 0; Y < Height; Y++)
    {
        for (i32 X = 0; X < Width; X++)
        {
            fread(Pixel, 3, 1, File);
            
            *P = Pixel[3];
            P++;
            *P = Pixel[2];
            P++;
            *P = Pixel[1];
            P++;
            *P = 0xFF;
            P++;
        }
        if (Width * 3 % 4 != 0)
            fseek(File, 4 - (Width * 3 % 4), SEEK_CUR);
    }
}

internal
void Read16(FILE* File, u8* Dest, bitmap_palette* Palette, i32 Width, i32 Height)
{
    u16 Pixel, Red, Green, Blue;
    u16 Bitmask = 0x1F;
    u8* P = Dest;
    
    for (i32 Y = 0; Y < Height; Y++)
    {
        for (i32 X = 0; X < Width; X++)
        {
            fread(&Pixel, 2, 1, File);
            Blue = (Pixel >> 10) & Bitmask;
            Green = (Pixel >> 5) & Bitmask;
            Red = Pixel & Bitmask;
            
            *P = Red * 8;
            P++;
            *P = Green * 8;
            P++;
            *P = Blue * 8;
            P++;
            *P = 0xFF;
            P++;
        }
        if ((2 * Width) % 4 != 0)
            fseek(File, 4 - ((2 * Width) % 4), SEEK_CUR);
    }
}

internal
void Read8(FILE* File, u8* Dest, bitmap_palette* Palette, i32 Width, i32 Height)
{
    u8 Pixel, *P;
    P = Dest;
    
    for (i32 Y = 0; Y < Height; Y++)
    {
        for (i32 X = 0; X < Width; X++)
        {
            fread(&Pixel, 1, 1, File);
            
            *P = Palette[Pixel].Red;
            P++;
            *P = Palette[Pixel].Green;
            P++;
            *P = Palette[Pixel].Blue;
            P++;
            *P = 0xFF;
            P++;
        }
        if (Width % 4 != 0)
            fseek(File, 4 - (Width % 4), SEEK_CUR);
    }
}

internal
void Read4(FILE* File, u8* Dest, bitmap_palette* Palette, i32 Width, i32 Height)
{
    i32 Size = (Width + 1) / 2;
    u8* RowStride = PlatformState.HeapAlloc(Size);
    u8 Index, Byte, *P;
    u8 Bitmask = 0x0F;
    
    P = Dest;
    for (i32 Y = 0; Y < Height; Y++)
    {
        fread(RowStride, Size, 1, File);
        for (i32 X = 0; X < Width; X++)
        {
            Byte = RowStride[X / 2];
            Index = (X % 2) ? Bitmask & Byte : Byte >> 4;
            
            *P = Palette[Index].Red;
            P++;
            *P = Palette[Index].Green;
            P++;
            *P = Palette[Index].Blue;
            P++;
            *P = 0xFF;
            P++;
        }
        if (Size % 4 != 0)
            fseek(File, 4 - (Size % 4), SEEK_CUR);
    }
    
    PlatformState.HeapFree(RowStride);
}

internal
void Read1(FILE* File, u8* Dest, bitmap_palette* Palette, i32 Width, i32 Height)
{
    i32 Size = (Width + 7) / 8;
    u8* RowStride = PlatformState.HeapAlloc(Size);
    u8 Index, Byte, *P;
    u8 Bitmask = 0x01;
    i32 Bit;
    
    P = Dest;
    for (i32 Y = 0; Y < Height; Y++)
    {
        fread(RowStride, Size, 1, File);
        for (i32 X = 0; X < Width; X++)
        {
            Bit = (X % 8) + 1;
            Byte = RowStride[X / 8];
            Index = Byte >> (8 - Bit);
            Index &= Bitmask;
            
            *P = Palette[Index].Red;
            P++;
            *P = Palette[Index].Green;
            P++;
            *P = Palette[Index].Blue;
            P++;
            *P = 0xFF;
            P++;
        }
        if (Size % 4 != 0)
            fseek(File, 4 - (Size % 4), SEEK_CUR);
    }
    
    PlatformState.HeapFree(RowStride);
}

void LoadBMP(const char* Path, loaded_image* Image)
{
    FILE* File;
    i16 MagicNumber = 0;
    bmp_header Header;
    bitmap_info Info;
    bitmap_palette* Palette = NULL;
    
    File = fopen(Path, "rb");
    Assert(File);
    
    fread(&MagicNumber, 2, 1, File);
    Assert(MagicNumber == BITMAP_MAGIC_NUMBER);
    
    fread((void*)&Header, 12, 1, File);
    fread((void*)&Info, 40, 1, File);
    
    // Checks
    Assert(Info.HeaderSize == 40 || Info.HeaderSize == 108 || Info.HeaderSize == 124);
    Assert(Info.NumPlanes == 1);
    Assert(Info.Compression == 4 || Info.Compression == 5);
    Assert(Info.Height > 0);
    
    // Load palette
    if (Info.BitsPerPixel <= 8)
    {
        fseek(File, 14 + Info.HeaderSize, SEEK_SET);
        if ((Info.BitsPerPixel == 1) && (Info.ColorsUsed == 0)) Info.ColorsUsed = 2;
        if ((Info.BitsPerPixel == 4) && (Info.ColorsUsed == 0)) Info.ColorsUsed = 16;
        if ((Info.BitsPerPixel == 8) && (Info.ColorsUsed == 0)) Info.ColorsUsed = 256;
        Palette = PlatformState.HeapAlloc(Info.ColorsUsed * sizeof(bitmap_palette));
        fread((void*)Palette, sizeof(bitmap_palette), Info.ColorsUsed, File);
    }
    
    Image->Data = PlatformState.HeapAlloc(Info.Width * Info.Height * 4);
    memset(Image->Data, 0x00, Info.Width * Info.Height * 4);
    
    fseek(File, Header.Offset, SEEK_SET);
    switch (Info.BitsPerPixel)
    {
        case 1:
        {
            Read1(File, Image->Data, Palette, Info.Width, Info.Height);
        } break;
        
        case 4:
        {
            if (Info.Compression == 2)
                ReadRLE4(File, Image->Data, Palette, Info.Width, Info.Height);
            else
                Read4(File, Image->Data, Palette, Info.Width, Info.Height);
        } break;
        
        case 8:
        {
            if (Info.Compression == 1)
                ReadRLE8(File, Image->Data, Palette, Info.Width, Info.Height);
            else
                Read8(File, Image->Data, Palette, Info.Width, Info.Height);
        } break;
        
        case 16:
        {
            Read16(File, Image->Data, Palette, Info.Width, Info.Height);
        } break;
        
        case 24:
        {
            Read24(File, Image->Data, Palette, Info.Width, Info.Height);
        } break;
        
        case 32:
        {
            Read32(File, Image->Data, Palette, Info.Width, Info.Height);
        } break;
    }
    
    Image->Width = Info.Width;
    Image->Height = Info.Height;
    
    if (Info.ColorsUsed) PlatformState.HeapFree(Palette);
    fclose(File);
}

void FreeBMP(loaded_image* Image)
{
    PlatformState.HeapFree(Image->Data);
}