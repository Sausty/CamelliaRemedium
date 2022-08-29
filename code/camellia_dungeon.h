/* date = August 29th 2022 3:58 am */

#ifndef CAMELLIA_DUNGEON_H
#define CAMELLIA_DUNGEON_H

#include "camellia_platform.h"

#define DUNGEON_WIDTH 52
#define DUNGEON_HEIGHT 32

typedef enum generator_type {
    GeneratorType_StandardHalf = 0x01,
    GeneratorType_DenseInside = 0x02,
    GeneratorType_DenseOutside = 0x03,
    GeneratorType_FiveRoomLine = 0x04,
    GeneratorType_FiveRoomCross = 0x05,
    GeneratorType_StandardFull = 0x06,
    GeneratorType_MiddleRoom = 0x07,
    GeneratorType_Circular = 0x08,
    GeneratorType_StandardThird = 0x09,
    GeneratorType_Max = 0x0A
} generator_type;

enum tile_type
{
    TileType_HardBorder = 0x01,
    TileType_SoftBorder = 0x02,
    TileType_Floor = 0x03,
    TileType_Wall = 0x04
};

typedef struct dungeon_generation_info {
    generator_type Type;
    generator_type TypeIfFail;
    i32 VerticalSectors;
    i32 HorizontalSectors;
    i32 RoomDensity;
} dungeon_generation_info;

typedef struct generated_dungeon {
    u16 Tiles[DUNGEON_WIDTH][DUNGEON_HEIGHT];
} generated_dungeon;

void GenerateDungeon(dungeon_generation_info* Info, generated_dungeon* Dungeon);

#endif //CAMELLIA_DUNGEON_H
