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
    HRESULT Result = D3D12.Device->lpVtbl->CreateFence(D3D12.Device, Fence->FenceValue, D3D12_FENCE_FLAG_NONE, &IID_ID3D12Fence, (void**)&Fence->Fence);
    Assert(SUCCEEDED(Result));
}

internal
void FenceFree(d3d12_fence* Fence)
{
    Fence->Fence->lpVtbl->Release(Fence->Fence);
}

internal
u64 FenceSignal(d3d12_fence* Fence)
{
    ++Fence->FenceValue;
    Fence->Fence->lpVtbl->Signal(Fence->Fence, Fence->FenceValue);
    return Fence->FenceValue;
}

internal
bool32 FenceReached(d3d12_fence* Fence, u64 Value)
{
    return(Fence->Fence->lpVtbl->GetCompletedValue(Fence->Fence) >= Value);
}

internal
void FenceSync(d3d12_fence* Fence, u64 Value)
{
    if (Fence->Fence->lpVtbl->GetCompletedValue(Fence->Fence) < Value)
    {
        Fence->Fence->lpVtbl->SetEventOnCompletion(Fence->Fence, Value, NULL);
    }
}

internal
void RendererGetWindowDimension(HWND Window, u32* Width, u32* Height)
{
    RECT ClientRect;
    GetClientRect(Window, &ClientRect);
    *Width = ClientRect.right - ClientRect.left;
    *Height = ClientRect.bottom - ClientRect.top;
}

internal
void DeviceFlush(d3d12_fence* Fence)
{
    FenceSync(Fence, FenceSignal(Fence));
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

void RendererInit(HWND Window)
{
    D3D12.RenderWindow = Window;
    
    RendererLoadDXGI();
    RendererLoadD3D12();
    
    HRESULT Result = D3D12GetDebugInterface(&IID_ID3D12Debug1, (void**)&D3D12.Debug);
    Assert(SUCCEEDED(Result));
    D3D12.Debug->lpVtbl->EnableDebugLayer(D3D12.Debug);
    
    Result = CreateDXGIFactory(&IID_IDXGIFactory, (void**)&D3D12.Factory);
    Assert(SUCCEEDED(Result));
    D3D12.Factory->lpVtbl->EnumAdapters(D3D12.Factory, 0, &D3D12.Adapter);
    
    Result = D3D12CreateDevice((IUnknown*)D3D12.Adapter, D3D_FEATURE_LEVEL_12_0, &IID_ID3D12Device, (void**)&D3D12.Device);
    Assert(SUCCEEDED(Result));
    
    Result = D3D12.Device->lpVtbl->QueryInterface(D3D12.Device, &IID_ID3D12DebugDevice, (void**)&D3D12.DebugDevice);
    Assert(SUCCEEDED(Result));
    
    D3D12.Adapter->lpVtbl->GetParent(D3D12.Adapter, &IID_IDXGIFactory, (void**)&D3D12.Factory);
    
    ID3D12InfoQueue* InfoQueue = 0;
    D3D12.Device->lpVtbl->QueryInterface(D3D12.Device, &IID_ID3D12InfoQueue, (void**)&InfoQueue);
    
    InfoQueue->lpVtbl->SetBreakOnSeverity(InfoQueue, D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
    InfoQueue->lpVtbl->SetBreakOnSeverity(InfoQueue, D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
    
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
    
    InfoQueue->lpVtbl->PushStorageFilter(InfoQueue, &Filter);
    InfoQueue->lpVtbl->Release(InfoQueue);
    
    D3D12_COMMAND_QUEUE_DESC QueueDesc = {0};
    Result = D3D12.Device->lpVtbl->CreateCommandQueue(D3D12.Device, &QueueDesc, &IID_ID3D12CommandQueue, (void**)&D3D12.Queue);
    Assert(SUCCEEDED(Result));
    
    FenceInit(&D3D12.DeviceFence);
    
    DXGI_SWAP_CHAIN_DESC1 SwapchainDesc = {0};
    RendererGetWindowDimension(D3D12.RenderWindow, &SwapchainDesc.Width, &SwapchainDesc.Height);
    SwapchainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    SwapchainDesc.SampleDesc.Count = 1;
    SwapchainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    SwapchainDesc.BufferCount = FRAMES_IN_FLIGHT;
    SwapchainDesc.Scaling = DXGI_SCALING_NONE;
    SwapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    
    IDXGISwapChain1* Temp = 0;
    D3D12.Factory->lpVtbl->CreateSwapChainForHwnd(D3D12.Factory, (IUnknown*)D3D12.Queue, D3D12.RenderWindow, &SwapchainDesc, NULL, NULL, &Temp);
    Temp->lpVtbl->QueryInterface(Temp, &IID_IDXGISwapChain3, (void**)&D3D12.Swapchain);
    Temp->lpVtbl->Release(Temp);
    
    for (u32 BufferIndex = 0; BufferIndex < FRAMES_IN_FLIGHT; BufferIndex++) 
    {
        D3D12.Swapchain->lpVtbl->GetBuffer(D3D12.Swapchain, BufferIndex, &IID_ID3D12Resource, (void**)&D3D12.SwapchainBuffers[BufferIndex]);
    }
}

void RendererExit()
{
    DeviceFlush(&D3D12.DeviceFence);
    
    for (u32 BufferIndex = 0; BufferIndex < FRAMES_IN_FLIGHT; BufferIndex++)
    {
        D3D12.SwapchainBuffers[BufferIndex]->lpVtbl->Release(D3D12.SwapchainBuffers[BufferIndex]);
    }
    
    SafeRelease(D3D12.Swapchain);
    FenceFree(&D3D12.DeviceFence);
    SafeRelease(D3D12.Queue);
    SafeRelease(D3D12.Device);
    SafeRelease(D3D12.Factory);
    SafeRelease(D3D12.Adapter);
    
    D3D12.DebugDevice->lpVtbl->ReportLiveDeviceObjects(D3D12.DebugDevice, D3D12_RLDO_IGNORE_INTERNAL | D3D12_RLDO_DETAIL);
    SafeRelease(D3D12.DebugDevice);
    SafeRelease(D3D12.Debug);
}