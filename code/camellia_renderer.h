/* date = August 28th 2022 5:06 pm */

#ifndef CAMELLIA_RENDERER_H
#define CAMELLIA_RENDERER_H

#include "camellia_platform.h"

#include "camellia_math.h"

#if defined(RENDERER_D3D12)
	#include "camellia_renderer_d3d12.h"
#endif

typedef struct renderer_begin {
	m4 View;
	m4 Projection;
} renderer_begin;

typedef struct renderer_state {
    void (*InitBuffer)(u64 BufferSize, u64 BufferStride, gpu_buffer_usage Usage, gpu_buffer* Buffer);
    void (*UploadBuffer)(u64 BufferSize, const void* Data, gpu_buffer* Buffer);
    void (*FreeBuffer)(gpu_buffer* Buffer);
    
    void (*Init)(void* WindowHandle);
    void (*Exit)();

    void (*Begin)(renderer_begin* Begin);
    void (*End)();

    void (*BindBuffer)(gpu_buffer* Buffer);

	void (*PushTransform)(m4 Transform);
    void (*Draw)(u32 VertexCount);
	void (*DrawIndexed)(u32 IndexCount);

    void (*Wait)();
    void (*Resize)(u32 Width, u32 Height);
} renderer_state;

extern renderer_state Renderer;

void RendererLoad();

#endif //CAMELLIA_RENDERER_H
