#include "camellia.h"

#include "camellia_renderer.h"

typedef struct game_state {
    gpu_buffer VertexBuffer;
} game_state;

static const f32 Vertices[] = {
    -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
    0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f
};

global game_state GameState;

void GameInit()
{
    Renderer.InitBuffer(sizeof(Vertices), sizeof(f32) * 6, GpuBufferUsage_Vertex, &GameState.VertexBuffer);
    Renderer.UploadBuffer(sizeof(Vertices), Vertices, &GameState.VertexBuffer);
}

void GameUpdate()
{
    Renderer.BindBuffer(&GameState.VertexBuffer);
    Renderer.Draw(3);
}

void GameFree()
{
    Renderer.FreeBuffer(&GameState.VertexBuffer);
}