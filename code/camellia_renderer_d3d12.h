/* date = August 13th 2022 8:38 am */

#ifndef CAMELLIA_RENDERER_D3D12_H
#define CAMELLIA_RENDERER_D3D12_H

#define COBJMACROS
#include <d3d12.h>
#include <dxgi1_6.h>

#include "camellia_platform.h"

#define FRAMES_IN_FLIGHT 3

typedef u32 d3d12_descriptor_handle;

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
    
    ID3D12CommandAllocator* CommandAllocators[FRAMES_IN_FLIGHT];
    ID3D12GraphicsCommandList* CommandLists[FRAMES_IN_FLIGHT];
    
    IDXGISwapChain3* Swapchain;
    ID3D12Resource* SwapchainBuffers[FRAMES_IN_FLIGHT];
    d3d12_descriptor_handle SwapchainRenderTargets[FRAMES_IN_FLIGHT];
    u64 FrameSync[FRAMES_IN_FLIGHT];
    u32 FrameIndex;
} d3d12_state;

void RendererInit(HWND Window);
void RendererExit();
void RendererRender();
void RendererResize(u32 Width, u32 Height);

#endif //CAMELLIA_RENDERER_D3D12_H
