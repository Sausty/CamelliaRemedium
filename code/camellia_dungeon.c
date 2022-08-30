#include "camellia_dungeon.h"
#include "camellia_math.h"

typedef struct generation_info {
    u32 RoomsToGenerate;
    dungeon_generation_info* Info;
} generation_info;

internal
void GenerateStandardHalf(generation_info* Info, generated_dungeon* Dungeon)
{   
}

internal
void GenerateDenseInside(generation_info* Info, generated_dungeon* Dungeon)
{    
}

internal
void GenerateDenseOutside(generation_info* Info, generated_dungeon* Dungeon)
{    
}

internal
void GenerateFiveRoomLine(generation_info* Info, generated_dungeon* Dungeon)
{    
}

internal
void GenerateFiveRoomCross(generation_info* Info, generated_dungeon* Dungeon)
{    
}

internal
void GenerateStandardFull(generation_info* Info, generated_dungeon* Dungeon)
{    
}

internal
void GenerateMiddleRoom(generation_info* Info, generated_dungeon* Dungeon)
{    
}

internal
void GenerateCircular(generation_info* Info, generated_dungeon* Dungeon)
{    
}

internal
void GenerateNone(generation_info* Info, generated_dungeon* Dungeon)
{
    for (i32 TileX = 0; TileX < DUNGEON_WIDTH; TileX++)
    {
        for (i32 TileY = 0; TileY < DUNGEON_HEIGHT; TileY++)
        {
            if (Dungeon->Tiles[TileX][TileY] == TileType_None)
                Dungeon->Tiles[TileX][TileY] = TileType_Room;
        }
    }
}

void GenerateDungeon(dungeon_generation_info* Info, generated_dungeon* Dungeon)
{
    // Generate hard borders
    for (i32 TileX = 0; TileX < DUNGEON_WIDTH; TileX++)
    {
        Dungeon->Tiles[TileX][0] = TileType_HardBorder;
        Dungeon->Tiles[TileX][DUNGEON_HEIGHT - 1] = TileType_HardBorder;      
    }
    
    for (i32 TileY = 0; TileY < DUNGEON_HEIGHT; TileY++)
    {
        Dungeon->Tiles[0][TileY] = TileType_HardBorder;
        Dungeon->Tiles[DUNGEON_WIDTH - 1][TileY] = TileType_HardBorder;
    }
    
    // Generate rooms
    u32 HorizontalSectorSize = DUNGEON_WIDTH / Info->HorizontalSectors;
    u32 VerticalSectorSize = DUNGEON_HEIGHT / Info->VerticalSectors;
    
    u32 RoomsToGenerate = 0;
    if (Info->RoomDensity < 0)
        RoomsToGenerate = abs(Info->RoomDensity);
    else
        RoomsToGenerate = Random(Info->RoomDensity, Minimum(Info->RoomDensity + 2, Info->HorizontalSectors + Info->VerticalSectors));
    
    generation_info GenerationInfo;
    GenerationInfo.RoomsToGenerate = RoomsToGenerate;
    GenerationInfo.Info = Info;
    
    switch (Info->Type)
    {
        case GeneratorType_StandardHalf:
        {
            GenerateStandardHalf(&GenerationInfo, Dungeon);
        } break;
        case GeneratorType_DenseInside:
        {
            GenerateDenseInside(&GenerationInfo, Dungeon);
        } break;
        case GeneratorType_DenseOutside:
        {
            GenerateDenseOutside(&GenerationInfo, Dungeon);
        } break;
        case GeneratorType_FiveRoomLine:
        {
            GenerateFiveRoomLine(&GenerationInfo, Dungeon);
        } break;
        case GeneratorType_FiveRoomCross:
        {
            GenerateFiveRoomCross(&GenerationInfo, Dungeon);
        } break;
        case GeneratorType_StandardFull:
        {
            GenerateStandardFull(&GenerationInfo, Dungeon);
        } break;
        case GeneratorType_MiddleRoom:
        {
            GenerateMiddleRoom(&GenerationInfo, Dungeon);
        } break;
        case GeneratorType_Circular:
        {
            GenerateCircular(&GenerationInfo, Dungeon);
        } break;
        case GeneratorType_None:
        {
            GenerateNone(&GenerationInfo, Dungeon);
        } break;
    }
}