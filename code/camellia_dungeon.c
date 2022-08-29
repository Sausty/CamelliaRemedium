#include "camellia_dungeon.h"

void GenerateDungeon(dungeon_generation_info* Info, generated_dungeon* Dungeon)
{
    // Generate hard borders
    for (i32 TileX = 0; TileX < DUNGEON_WIDTH; TileX++)
    {
        for (i32 TileY = 0; TileY < DUNGEON_HEIGHT; TileY++)
        {
            // Top
            Dungeon->Tiles[TileX][0] = TileType_HardBorder;
            // Left
            Dungeon->Tiles[0][TileY] = TileType_HardBorder;
            // Right
            Dungeon->Tiles[DUNGEON_WIDTH - 1][TileY] = TileType_HardBorder;
            // Bottom
            Dungeon->Tiles[TileX][DUNGEON_HEIGHT - 1] = TileType_HardBorder;
        }
    }
    
    // Generate soft borders
    for (i32 TileX = 1; TileX < DUNGEON_WIDTH - 1; TileX++)
    {
        for (i32 TileY = 1; TileY < DUNGEON_HEIGHT - 1; TileY++)
        {
            // Top
            Dungeon->Tiles[TileX][1] = TileType_SoftBorder;
            // Left
            Dungeon->Tiles[1][TileY] = TileType_SoftBorder;
            // Right
            Dungeon->Tiles[DUNGEON_WIDTH - 2][TileY] = TileType_SoftBorder;
            // Bottom
            Dungeon->Tiles[TileX][DUNGEON_HEIGHT - 2] = TileType_SoftBorder;
        }
    } 
}