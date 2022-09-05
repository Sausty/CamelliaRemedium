/* date = August 13th 2022 8:38 am */

#ifndef CAMELLIA_RENDERER_D3D12_H
#define CAMELLIA_RENDERER_D3D12_H

#define COBJMACROS
#include <d3d12.h> 
#include <dxgi1_6.h>

#include "camellia_platform.h"
#include "camellia_math.h"

#define FRAMES_IN_FLIGHT 3

typedef u32 d3d12_descriptor_handle;
typedef struct renderer_begin renderer_begin;

typedef enum gpu_buffer_usage {
    GpuBufferUsage_Vertex = 0,
    GpuBufferUsage_Index = 1,
    GpuBufferUsage_Constant = 2
} gpu_buffer_usage;

typedef struct d3d12_fence {
    ID3D12Fence* Fence;
    u32 FenceValue;
} d3d12_fence;

typedef struct d3d12_descriptor_heap {
    ID3D12DescriptorHeap* Heap;
    u32 IncrementSize;
    u32 DescriptorCount;
    D3D12_DESCRIPTOR_HEAP_TYPE Type;
    
    bool32* DescriptorLUT;
} d3d12_descriptor_heap;

typedef struct d3d12_graphics_pipeline {
    ID3D12RootSignature* RootSignature;
    ID3D12PipelineState* PipelineState;
} d3d12_graphics_pipeline;

typedef struct gpu_buffer {
    ID3D12Resource* Resource;
    u64 BufferSize;
    u64 BufferStride;
	u32 HeapIndex;
    gpu_buffer_usage Usage;
    
    D3D12_VERTEX_BUFFER_VIEW VertexBufferView;
    D3D12_INDEX_BUFFER_VIEW IndexBufferView;
    D3D12_CONSTANT_BUFFER_VIEW_DESC ConstantBufferView;
} gpu_buffer;

typedef struct d3d12_state {
    HWND RenderWindow;
    
    IDXGIDevice* DXGIDevice;
    IDXGIFactory3* Factory;
    IDXGIAdapter1* Adapter;
    ID3D12Device* Device;
    ID3D12Debug1* Debug;
    ID3D12DebugDevice* DebugDevice;
    
    ID3D12CommandQueue* Queue;
    d3d12_fence DeviceFence;
    
    d3d12_descriptor_heap RenderTargetViewHeap;
	d3d12_descriptor_heap ShaderHeap;
    
    ID3D12CommandAllocator* CommandAllocators[FRAMES_IN_FLIGHT];
    ID3D12GraphicsCommandList* CommandLists[FRAMES_IN_FLIGHT];
    
    IDXGISwapChain3* Swapchain;
    ID3D12Resource* SwapchainBuffers[FRAMES_IN_FLIGHT];
    d3d12_descriptor_handle SwapchainRenderTargets[FRAMES_IN_FLIGHT];
    u64 FrameSync[FRAMES_IN_FLIGHT];
    u32 FrameIndex;
    
    d3d12_graphics_pipeline ForwardPipeline;
	gpu_buffer CameraBuffer;
} d3d12_state;

void D3D12InitBuffer(u64 BufferSize, u64 BufferStride, gpu_buffer_usage Usage, gpu_buffer* Buffer);
void D3D12UploadBuffer(u64 BufferSize, const void* Data, gpu_buffer* Buffer);
void D3D12FreeBuffer(gpu_buffer* Buffer);

void D3D12Init(void* Window);
void D3D12Exit();

void D3D12Begin(renderer_begin* Begin);
void D3D12End();

void D3D12BindBuffer(gpu_buffer* Buffer);

void D3D12PushTransform(m4 Transform);
void D3D12Draw(u32 VertexCount);
void D3D12DrawIndexed(u32 IndexCount);

void D3D12Wait();
void D3D12Resize(u32 Width, u32 Height);

#endif //CAMELLIA_RENDERER_D3D12_H
