#include "camellia_renderer_d3d12.h"

#define SafeRelease(obj) if (obj) { obj->lpVtbl->Release(obj); }

#define D3D12_CREATE_DEVICE(name) HRESULT name(IUnknown *pAdapter, D3D_FEATURE_LEVEL MinimumFeatureLevel, REFIID riid, void **ppDevice)
typedef D3D12_CREATE_DEVICE(d3d12_create_device);

#define D3D12_GET_DEBUG_INTERFACE(name) HRESULT name(REFIID riid, void **ppvDebug)
typedef D3D12_GET_DEBUG_INTERFACE(d3d12_get_debug_interface);

#define CREATE_DXGI_FACTORY(name) HRESULT name(REFIID riid, void **ppFactory)
typedef CREATE_DXGI_FACTORY(create_dxgi_factory);

D3D12_CREATE_DEVICE(D3D12CreateDeviceStub) { return S_FALSE; }
D3D12_GET_DEBUG_INTERFACE(D3D12GetDebugInterfaceStub) { return S_FALSE; }
CREATE_DXGI_FACTORY(CreateDXGIFactoryStub) { return S_FALSE; }

global d3d12_create_device *d3d12CreateDevice_ = D3D12CreateDeviceStub;
global d3d12_get_debug_interface *d3d12GetDebugInterface_ = D3D12GetDebugInterfaceStub;
global create_dxgi_factory *createDXGIFactory_ = CreateDXGIFactoryStub;

#define D3D12CreateDevice d3d12CreateDevice_
#define D3D12GetDebugInterface d3d12GetDebugInterface_
#define CreateDXGIFactory createDXGIFactory_

global d3d12_state D3D12;

internal
void FenceInit(d3d12_fence* Fence)
{
    HRESULT Result = ID3D12Device_CreateFence(D3D12.Device, Fence->FenceValue, D3D12_FENCE_FLAG_NONE, &IID_ID3D12Fence, (void**)&Fence->Fence);
    Assert(SUCCEEDED(Result));
    
    Fence->FenceValue = 0;
}

internal
void SignalFence(d3d12_fence* Fence)
{
    ID3D12CommandQueue_Signal(D3D12.Queue, Fence->Fence, ++Fence->FenceValue);
}

internal
void WaitFence(d3d12_fence* Fence)
{
    if (ID3D12Fence_GetCompletedValue(Fence->Fence) < Fence->FenceValue) 
    {
        HANDLE Handle = CreateEvent(NULL, false, false, NULL);
        ID3D12Fence_SetEventOnCompletion(Fence->Fence, Fence->FenceValue, Handle);
        WaitForSingleObject(Fence->Fence, INFINITE);
        CloseHandle(Handle);
    }
}

internal
void FenceFree(d3d12_fence* Fence)
{
    SafeRelease(Fence->Fence);
}

internal
void DescriptorHeapInit(d3d12_descriptor_heap* Heap, D3D12_DESCRIPTOR_HEAP_TYPE Type, u32 DescriptorCount)
{
    Heap->Type = Type;
    Heap->DescriptorCount = DescriptorCount;
    
    D3D12_DESCRIPTOR_HEAP_DESC HeapDesc = {0};
    HeapDesc.NumDescriptors = DescriptorCount;
    HeapDesc.Type = Type;
    HeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    ID3D12Device_CreateDescriptorHeap(D3D12.Device, &HeapDesc, &IID_ID3D12DescriptorHeap, (void**)&Heap->Heap);
    
    Heap->IncrementSize = ID3D12Device_GetDescriptorHandleIncrementSize(D3D12.Device, Type);
    Heap->DescriptorLUT = malloc(sizeof(bool32) * DescriptorCount);
    memset(Heap->DescriptorLUT, 0, sizeof(bool32) * DescriptorCount);
}

internal 
void DescriptorHeapFree(d3d12_descriptor_heap* Heap)
{
    free(Heap->DescriptorLUT);
    SafeRelease(Heap->Heap);
}

internal
D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeapCPU(d3d12_descriptor_heap* Heap, u32 Offset)
{
    D3D12_CPU_DESCRIPTOR_HANDLE CPU = {0};
    ID3D12DescriptorHeap_GetCPUDescriptorHeapHandleForHeapStart(Heap->Heap, &CPU);
    CPU.ptr += Offset * Heap->IncrementSize;
    return(CPU);
}

internal
D3D12_GPU_DESCRIPTOR_HANDLE DescriptorHeapGPU(d3d12_descriptor_heap* Heap, u32 Offset)
{
    D3D12_GPU_DESCRIPTOR_HANDLE GPU = {0};
    ID3D12DescriptorHeap_GetGPUDescriptorHandleForHeapStart(Heap->Heap, &GPU);
    GPU.ptr += Offset * Heap->IncrementSize;
    return(GPU);
}

internal
u32 AllocDescriptor(d3d12_descriptor_heap* Heap)
{
    for (u32 DescriptorIndex = 0; DescriptorIndex < Heap->DescriptorCount; DescriptorIndex++)
    {
        if (Heap->DescriptorLUT[DescriptorIndex] == false)
        {
            Heap->DescriptorLUT[DescriptorIndex] = true;
            return DescriptorIndex;
        }
    }
    
    return 0;
}

internal
void FreeDescriptor(d3d12_descriptor_heap* Heap, u32 Descriptor)
{
    Heap->DescriptorLUT[Descriptor] = false;
}

internal
void RendererGetWindowDimension(HWND Window, u32* Width, u32* Height)
{
    RECT ClientRect;
    GetClientRect(Window, &ClientRect);
    *Width = ClientRect.right - ClientRect.left;
    *Height = ClientRect.bottom - ClientRect.top;
}

void RendererLoadDXGI()
{
    HMODULE Library = LoadLibraryA("dxgi.dll");
    if (Library)
    {
        CreateDXGIFactory = (create_dxgi_factory*)GetProcAddress(Library, "CreateDXGIFactory");
    }
}

void RendererLoadD3D12()
{
    HMODULE Library = LoadLibraryA("d3d12.dll");
    if (Library)
    {
        D3D12CreateDevice = (d3d12_create_device*)GetProcAddress(Library, "D3D12CreateDevice");
        D3D12GetDebugInterface = (d3d12_get_debug_interface*)GetProcAddress(Library, "D3D12GetDebugInterface");
    }
}

internal
void GetHardwareAdapter(IDXGIFactory3* Factory, IDXGIAdapter1** RetAdapter, bool32 RequestHighPerformanceAdapter)
{
    *RetAdapter = NULL;
    IDXGIAdapter1* Adapter = 0;
    IDXGIFactory6* Factory6;
    
    if (SUCCEEDED(IDXGIFactory3_QueryInterface(Factory, &IID_IDXGIFactory6, (void**)&Factory6)))
    {
        for (u32 AdapterIndex = 0; SUCCEEDED(IDXGIFactory6_EnumAdapterByGpuPreference(Factory6, AdapterIndex, RequestHighPerformanceAdapter == true ? DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE : DXGI_GPU_PREFERENCE_UNSPECIFIED, &IID_IDXGIAdapter1, (void**)&Adapter)); ++AdapterIndex) 
        {
            DXGI_ADAPTER_DESC1 Desc1;
            IDXGIAdapter1_GetDesc1(Adapter, &Desc1);
            
            if (Desc1.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
            {
                continue;
            }
            
            if (SUCCEEDED(D3D12CreateDevice((IUnknown*)Adapter, D3D_FEATURE_LEVEL_11_0, &IID_ID3D12Device, NULL)))
            {
                break;
            }
        }
    }
    
    if (Adapter == NULL)
    {
        for (u32 AdapterIndex = 0; SUCCEEDED(IDXGIFactory3_EnumAdapters1(Factory, AdapterIndex, &Adapter)); ++AdapterIndex)
        {
            DXGI_ADAPTER_DESC1 Desc1;
            IDXGIAdapter1_GetDesc1(Adapter, &Desc1);
            
            if (Desc1.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
            {
                continue;
            }
            
            if (SUCCEEDED(D3D12CreateDevice((IUnknown*)Adapter, D3D_FEATURE_LEVEL_11_0, &IID_ID3D12Device, NULL)))
            {
                break;
            }
        }
    }
    
    *RetAdapter = Adapter;
}

void RendererInit(HWND Window)
{
    D3D12.RenderWindow = Window;
    
    RendererLoadDXGI();
    RendererLoadD3D12();
    
    HRESULT Result = D3D12GetDebugInterface(&IID_ID3D12Debug1, (void**)&D3D12.Debug);
    Assert(SUCCEEDED(Result));
    ID3D12Debug1_EnableDebugLayer(D3D12.Debug);
    
    Result = CreateDXGIFactory(&IID_IDXGIFactory, (void**)&D3D12.Factory);
    Assert(SUCCEEDED(Result));
    GetHardwareAdapter(D3D12.Factory, &D3D12.Adapter, true);
    
    // Create device
    Result = D3D12CreateDevice((IUnknown*)D3D12.Adapter, D3D_FEATURE_LEVEL_12_0, &IID_ID3D12Device, (void**)&D3D12.Device);
    Assert(SUCCEEDED(Result));
    
    Result = ID3D12Device_QueryInterface(D3D12.Device, &IID_ID3D12DebugDevice, (void**)&D3D12.DebugDevice);
    Assert(SUCCEEDED(Result));
    
    IDXGIAdapter_GetParent(D3D12.Adapter, &IID_IDXGIFactory, (void**)&D3D12.Factory);
    
    // Create info queue
    
    ID3D12InfoQueue* InfoQueue = 0;
    ID3D12Device_QueryInterface(D3D12.Device, &IID_ID3D12InfoQueue, (void**)&InfoQueue);
    
    ID3D12InfoQueue_SetBreakOnSeverity(InfoQueue, D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
    ID3D12InfoQueue_SetBreakOnSeverity(InfoQueue, D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
    
    D3D12_MESSAGE_SEVERITY SupressSeverities[] =
    {
        D3D12_MESSAGE_SEVERITY_INFO
    };
    
    D3D12_MESSAGE_ID SupressIDs[] =
    {
        D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE,
        D3D12_MESSAGE_ID_CLEARDEPTHSTENCILVIEW_MISMATCHINGCLEARVALUE,
        D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,
        D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE,
    };
    
    D3D12_INFO_QUEUE_FILTER Filter = {0};
    Filter.DenyList.NumSeverities = ArrayCount(SupressSeverities);
    Filter.DenyList.pSeverityList = SupressSeverities;
    Filter.DenyList.NumIDs = ArrayCount(SupressIDs);
    Filter.DenyList.pIDList = SupressIDs;
    
    ID3D12InfoQueue_PushStorageFilter(InfoQueue, &Filter);
    ID3D12InfoQueue_Release(InfoQueue);
    
    // Create command queue
    D3D12_COMMAND_QUEUE_DESC QueueDesc = {0};
    Result = ID3D12Device_CreateCommandQueue(D3D12.Device, &QueueDesc, &IID_ID3D12CommandQueue, (void**)&D3D12.Queue);
    Assert(SUCCEEDED(Result));
    
    // Create Fence
    FenceInit(&D3D12.DeviceFence);
    
    // Create descriptor heaps
    DescriptorHeapInit(&D3D12.RenderTargetViewHeap, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 128);
    
    // Create Swap Chain
    DXGI_SWAP_CHAIN_DESC1 SwapchainDesc = {0};
    RendererGetWindowDimension(D3D12.RenderWindow, &SwapchainDesc.Width, &SwapchainDesc.Height);
    SwapchainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    SwapchainDesc.SampleDesc.Count = 1;
    SwapchainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    SwapchainDesc.BufferCount = FRAMES_IN_FLIGHT;
    SwapchainDesc.Scaling = DXGI_SCALING_NONE;
    SwapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    
    IDXGISwapChain1* Temp = 0;
    IDXGIFactory3_CreateSwapChainForHwnd(D3D12.Factory, (IUnknown*)D3D12.Queue, D3D12.RenderWindow, &SwapchainDesc, NULL, NULL, &Temp);
    IDXGISwapChain1_QueryInterface(Temp, &IID_IDXGISwapChain3, (void**)&D3D12.Swapchain);
    IDXGISwapChain1_Release(Temp);
    
    ID3D12Device_CreateCommandAllocator(D3D12.Device, D3D12_COMMAND_LIST_TYPE_DIRECT, &IID_ID3D12CommandAllocator, (void**)&D3D12.CommandAllocator);
    
    for (u32 BufferIndex = 0; BufferIndex < FRAMES_IN_FLIGHT; BufferIndex++)
    {
        // Get buffers
        IDXGISwapChain3_GetBuffer(D3D12.Swapchain, BufferIndex, &IID_ID3D12Resource, (void**)&D3D12.SwapchainBuffers[BufferIndex]);
        D3D12.SwapchainRenderTargets[BufferIndex] = AllocDescriptor(&D3D12.RenderTargetViewHeap);
        
        ID3D12Device_CreateRenderTargetView(D3D12.Device, D3D12.SwapchainBuffers[BufferIndex], NULL, DescriptorHeapCPU(&D3D12.RenderTargetViewHeap, D3D12.SwapchainRenderTargets[BufferIndex]));
        
        // Create command objects
        ID3D12Device_CreateCommandList(D3D12.Device, 0, D3D12_COMMAND_LIST_TYPE_DIRECT, D3D12.CommandAllocator, NULL, &IID_ID3D12GraphicsCommandList, (void**)&D3D12.CommandLists[BufferIndex]);
        ID3D12GraphicsCommandList_Close(D3D12.CommandLists[BufferIndex]);
        
        // Create sync
        FenceInit(&D3D12.FrameFences[BufferIndex]);
    }
    
    SignalFence(&D3D12.DeviceFence);
    WaitFence(&D3D12.DeviceFence);
}

void RendererExit()
{
    SignalFence(&D3D12.DeviceFence);
    WaitFence(&D3D12.DeviceFence);
    
    for (u32 BufferIndex = 0; BufferIndex < FRAMES_IN_FLIGHT; BufferIndex++)
    {
        SafeRelease(D3D12.CommandLists[BufferIndex]);
        
        FenceFree(&D3D12.FrameFences[BufferIndex]);
        FreeDescriptor(&D3D12.RenderTargetViewHeap, D3D12.SwapchainRenderTargets[BufferIndex]);
        ID3D12Resource_Release(D3D12.SwapchainBuffers[BufferIndex]);
    }
    SafeRelease(D3D12.CommandAllocator);
    
    SafeRelease(D3D12.Swapchain);
    DescriptorHeapFree(&D3D12.RenderTargetViewHeap);
    FenceFree(&D3D12.DeviceFence);
    SafeRelease(D3D12.Queue);
    SafeRelease(D3D12.Device);
    SafeRelease(D3D12.Factory);
    SafeRelease(D3D12.Adapter);
    
    ID3D12DebugDevice_ReportLiveDeviceObjects(D3D12.DebugDevice, D3D12_RLDO_IGNORE_INTERNAL | D3D12_RLDO_DETAIL);
    SafeRelease(D3D12.DebugDevice);
    SafeRelease(D3D12.Debug);
}

void RendererRender()
{
    u32 Width = 0;
    u32 Height = 0;
    RendererGetWindowDimension(D3D12.RenderWindow, &Width, &Height);
    
    if (Width == 0 || Height == 0)
    {
        return;
    }
    
    // Start
    D3D12.FrameIndex = IDXGISwapChain3_GetCurrentBackBufferIndex(D3D12.Swapchain);
    WaitFence(&D3D12.FrameFences[D3D12.FrameIndex]);
    
    ID3D12GraphicsCommandList* Command = D3D12.CommandLists[D3D12.FrameIndex];
    
    ID3D12CommandAllocator_Reset(D3D12.CommandAllocator);
    ID3D12GraphicsCommandList_Reset(Command, D3D12.CommandAllocator, NULL);
    
    // Prepare swapchain
    D3D12_VIEWPORT Viewport;
    ZeroMemory(&Viewport, sizeof(D3D12_VIEWPORT));
    Viewport.TopLeftX = 0;
    Viewport.TopLeftY = 0;
    Viewport.Width = Width;
    Viewport.Height = Height;
    
    D3D12_RECT ScissorRect;
    ScissorRect.left = 0;
    ScissorRect.top = 0;
    ScissorRect.right = Width;
    ScissorRect.bottom = Height;
    
    // Prepare command objects and render
    D3D12_RESOURCE_BARRIER Barrier;
    ZeroMemory(&Barrier, sizeof(D3D12_RESOURCE_BARRIER));
    Barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    Barrier.Transition.pResource = D3D12.SwapchainBuffers[D3D12.FrameIndex];
    Barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
    Barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
    ID3D12GraphicsCommandList_ResourceBarrier(Command, 1, &Barrier);
    
    ID3D12GraphicsCommandList_RSSetViewports(Command, 1, &Viewport);
    ID3D12GraphicsCommandList_RSSetScissorRects(Command, 1, &ScissorRect);
    
    D3D12_CPU_DESCRIPTOR_HANDLE RenderTarget = DescriptorHeapCPU(&D3D12.RenderTargetViewHeap, D3D12.SwapchainRenderTargets[D3D12.FrameIndex]);
    ID3D12GraphicsCommandList_OMSetRenderTargets(Command, 1, &RenderTarget, false, NULL);
    const f32 Clear[4] = { 0.1f, 0.3f, 0.3f, 1.0f };
    ID3D12GraphicsCommandList_ClearRenderTargetView(Command, RenderTarget, Clear, 0, NULL);
    
    Barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
    Barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
    ID3D12GraphicsCommandList_ResourceBarrier(Command, 1, &Barrier);
    
    ID3D12GraphicsCommandList_Close(Command);
    
    // Execute
    ID3D12CommandList* CommandLists[] = { (ID3D12CommandList*)Command };
    ID3D12CommandQueue_ExecuteCommandLists(D3D12.Queue, 1, CommandLists);
    
    // Present
    IDXGISwapChain3_Present(D3D12.Swapchain, true, 0);
    SignalFence(&D3D12.FrameFences[D3D12.FrameIndex]);
}

void RendererResize(u32 Width, u32 Height)
{
    if (D3D12.Swapchain)
    {
        SignalFence(&D3D12.DeviceFence);
        WaitFence(&D3D12.DeviceFence);
        
        for (u32 BufferIndex = 0; BufferIndex < FRAMES_IN_FLIGHT; BufferIndex++)
        {
            FreeDescriptor(&D3D12.RenderTargetViewHeap, D3D12.SwapchainRenderTargets[BufferIndex]);
            ID3D12Resource_Release(D3D12.SwapchainBuffers[BufferIndex]);
        }
        IDXGISwapChain3_ResizeBuffers(D3D12.Swapchain, 0, Width, Height, DXGI_FORMAT_UNKNOWN, 0);
        for (u32 BufferIndex = 0; BufferIndex < FRAMES_IN_FLIGHT; BufferIndex++)
        {
            IDXGISwapChain3_GetBuffer(D3D12.Swapchain, BufferIndex, &IID_ID3D12Resource, (void**)&D3D12.SwapchainBuffers[BufferIndex]);
            D3D12.SwapchainRenderTargets[BufferIndex] = AllocDescriptor(&D3D12.RenderTargetViewHeap);
            
            ID3D12Device_CreateRenderTargetView(D3D12.Device, D3D12.SwapchainBuffers[BufferIndex], NULL, DescriptorHeapCPU(&D3D12.RenderTargetViewHeap, D3D12.SwapchainRenderTargets[BufferIndex]));
        }
    }
}