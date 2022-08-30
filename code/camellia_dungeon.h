/* date = August 29th 2022 3:58 am */

#ifndef CAMELLIA_DUNGEON_H
#define CAMELLIA_DUNGEON_H

#include "camellia_platform.h"

#define DUNGEON_WIDTH 56
#define DUNGEON_HEIGHT 32
#define MIN_ROOM_WIDTH 5
#define MIN_ROOM_HEIGHT 4

// TODO(milo): Generate rooms
// TODO(milo): Generate corridors
// TODO(milo): Add connection tiles
// TODO(milo): Complete other algoriths
// TODO(milo): Add trap tiles
// TODO(milo): Add shops
// TODO(milo): Add stairs

typedef enum generator_type {
    GeneratorType_StandardHalf = 0x01,
    GeneratorType_DenseInside = 0x02,
    GeneratorType_DenseOutside = 0x03,
    GeneratorType_FiveRoomLine = 0x04,
    GeneratorType_FiveRoomCross = 0x05,
    GeneratorType_StandardFull = 0x06,
    GeneratorType_MiddleRoom = 0x07,
    GeneratorType_Circular = 0x08,
    GeneratorType_None = 0x09,
    GeneratorType_Max = 0x0A
} generator_type;

enum tile_type
{
    // No tile
    TileType_None = 0x00,
    // Cannot go through
    TileType_HardBorder = 0x01,
    // Can go through, room tile
    TileType_Room = 0x02,
    // Can go through, corridor tile
    TileType_Corridor = 0x03,
    // Can go through, room tile where a corridor tile is neighbour
    TileType_Connection = 0x04,
    // Can go through, takes you to the next floor
    TileType_Stairs = 0x05,
    // Can go through, shop tile
    TileType_Shop = 0x06
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
