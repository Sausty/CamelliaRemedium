#include "camellia.h"

#include "camellia_renderer.h"
#include "camellia_dungeon.h"
#include "camellia_math.h"

#include <Windows.h>

typedef struct geometry_vertex {
	v3 Position;
	v3 Color;
} geometry_vertex;

typedef struct game_state {
    gpu_buffer VertexBuffer;
	gpu_buffer IndexBuffer;

    generated_dungeon Dungeon;
} game_state;

global const geometry_vertex Vertices[] = {
	{ {-0.0868241f,   0.49240386f, 0.0f}, {1.0f, 0.0f, 0.0f} }, // A
	{ {-0.49513406f,  0.06958647f, 0.0f}, {0.0f, 0.0f, 1.0f} }, // B
	{ {-0.21918549f, -0.44939706f, 0.0f}, {0.0f, 1.0f, 0.0f} }, // C
	{ {0.35966998f,  -0.3473291f,  0.0f}, {0.0f, 1.0f, 1.0f} }, // D
	{ {0.44147372f,   0.2347359f,  0.0f}, {1.0f, 0.0f, 1.0f} }, // E
};

global const u32 Indices[] = {
	0, 1, 4,
	1, 2, 4,
	2, 3, 4,
};

global game_state GameState;

void GameInit()
{
    dungeon_generation_info DummyGeneration;
    DummyGeneration.Type = GeneratorType_StandardFull;
    DummyGeneration.TypeIfFail = GeneratorType_Circular;
    DummyGeneration.VerticalSectors = 2;
    DummyGeneration.HorizontalSectors = 3;
    DummyGeneration.RoomDensity = 2;
    GenerateDungeon(&DummyGeneration, &GameState.Dungeon);
    
    Renderer.InitBuffer(sizeof(Vertices), sizeof(geometry_vertex), GpuBufferUsage_Vertex, &GameState.VertexBuffer);
    Renderer.UploadBuffer(sizeof(Vertices), Vertices, &GameState.VertexBuffer);

	Renderer.InitBuffer(sizeof(Indices), sizeof(u32), GpuBufferUsage_Index, &GameState.IndexBuffer);
	Renderer.UploadBuffer(sizeof(Indices), Indices, &GameState.IndexBuffer);
}

void GameUpdate()
{
	renderer_begin Begin =
	{
		.View = M4Identity(),
		.Projection = M4Identity()
	};

	Renderer.Begin(&Begin);
    Renderer.BindBuffer(&GameState.VertexBuffer);
	Renderer.BindBuffer(&GameState.IndexBuffer);
	Renderer.PushTransform(M4Translate(V3New(0.3f, 0.3f, 0.0f)));
    Renderer.DrawIndexed(9);
	Renderer.End();
}

void GameFree()
{    
    Renderer.FreeBuffer(&GameState.VertexBuffer);
}