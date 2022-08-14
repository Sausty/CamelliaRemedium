/* date = August 13th 2022 8:38 am */

#ifndef CAMELLIA_RENDERER_D3D12_H
#define CAMELLIA_RENDERER_D3D12_H

#include "camellia_platform.h"

#include <d3d12.h>
#include <dxgi1_6.h>

#define FRAMES_IN_FLIGHT 3

typedef struct d3d12_fence {
    ID3D12Fence* Fence;
    u64 FenceValue;
} d3d12_fence;

typedef struct d3d12_state {
    HWND RenderWindow;
    
    IDXGIDevice* DXGIDevice;
    IDXGIFactory3* Factory;
    IDXGIAdapter* Adapter;
    ID3D12Device* Device;
    ID3D12Debug1* Debug;
    ID3D12DebugDevice* DebugDevice;
    
    ID3D12CommandQueue* Queue;
    d3d12_fence DeviceFence;
    
    IDXGISwapChain3* Swapchain;
    ID3D12Resource* SwapchainBuffers[FRAMES_IN_FLIGHT];
    u64 SwapchainFenceValues[FRAMES_IN_FLIGHT];
} d3d12_state;

void RendererInit(HWND Window);
void RendererExit();

#endif //CAMELLIA_RENDERER_D3D12_H
