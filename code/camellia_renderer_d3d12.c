#include "camellia_renderer_d3d12.h"
#include "camellia_renderer.h"

#include <stdio.h>

#define SafeRelease(obj) if (obj) { obj->lpVtbl->Release(obj); }

#define D3D12_CREATE_DEVICE(name) HRESULT name(IUnknown *pAdapter, D3D_FEATURE_LEVEL MinimumFeatureLevel, REFIID riid, void **ppDevice)
typedef D3D12_CREATE_DEVICE(d3d12_create_device);

#define D3D12_GET_DEBUG_INTERFACE(name) HRESULT name(REFIID riid, void **ppvDebug)
typedef D3D12_GET_DEBUG_INTERFACE(d3d12_get_debug_interface);

#define CREATE_DXGI_FACTORY(name) HRESULT name(REFIID riid, void **ppFactory)
typedef CREATE_DXGI_FACTORY(create_dxgi_factory);

#define D3D12_SERIALIZE_ROOT_SIGNATURE(name) HRESULT name(const D3D12_ROOT_SIGNATURE_DESC* pRootSignature, D3D_ROOT_SIGNATURE_VERSION Version, ID3DBlob **ppBlob, ID3DBlob **ppErrorBlob)
typedef D3D12_SERIALIZE_ROOT_SIGNATURE(d3d12_serialize_root_signature);

#define D3D_COMPILE(name) HRESULT name(LPCVOID pSrcData, SIZE_T SrcDataSize, LPCSTR pSourceName, const D3D_SHADER_MACRO *pDefines, ID3DInclude *pInclude, LPCSTR pEntrypoint, LPCSTR pTarget, UINT Flags1, UINT Flags2, ID3DBlob **ppCode, ID3DBlob **ppErrorMsgs)
typedef D3D_COMPILE(d3d_compile);

D3D12_CREATE_DEVICE(D3D12CreateDeviceStub) { return S_FALSE; }
D3D12_GET_DEBUG_INTERFACE(D3D12GetDebugInterfaceStub) { return S_FALSE; }
CREATE_DXGI_FACTORY(CreateDXGIFactoryStub) { return S_FALSE; }
D3D12_SERIALIZE_ROOT_SIGNATURE(D3D12SerializeRootSignatureStub) { return S_FALSE; }
D3D_COMPILE(D3DCompileStub) { return S_FALSE; }

global d3d12_create_device *d3d12CreateDevice_ = D3D12CreateDeviceStub;
global d3d12_get_debug_interface *d3d12GetDebugInterface_ = D3D12GetDebugInterfaceStub;
global create_dxgi_factory *createDXGIFactory_ = CreateDXGIFactoryStub;
global d3d12_serialize_root_signature *d3d12SerializeRootSignature_ = D3D12SerializeRootSignatureStub;
global d3d_compile *d3dCompile_ = D3DCompileStub;

#define D3D12CreateDevice d3d12CreateDevice_
#define D3D12GetDebugInterface d3d12GetDebugInterface_
#define CreateDXGIFactory createDXGIFactory_
#define D3D12SerializeRootSignature d3d12SerializeRootSignature_
#define D3DCompile d3dCompile_

global d3d12_state D3D12;

internal
void FenceInit(d3d12_fence* Fence)
{
    HRESULT Result = ID3D12Device_CreateFence(D3D12.Device, Fence->FenceValue, D3D12_FENCE_FLAG_NONE, &IID_ID3D12Fence, (void**)&Fence->Fence);
    Assert(SUCCEEDED(Result));
    
    Fence->FenceValue = 0;
}

internal
u64 FenceSignal(d3d12_fence* Fence)
{
    ++Fence->FenceValue;
    ID3D12CommandQueue_Signal(D3D12.Queue, Fence->Fence, Fence->FenceValue);
    return Fence->FenceValue;
}

internal
bool32 FenceReached(d3d12_fence* Fence, u64 Value)
{
    return ID3D12Fence_GetCompletedValue(Fence->Fence) >= Value;
}

internal
void FenceSync(d3d12_fence* Fence, u64 Value)
{
    if (!FenceReached(Fence, Value)) {
        ID3D12Fence_SetEventOnCompletion(Fence->Fence, Value, NULL);
    }
}

internal
void FenceFlush(d3d12_fence* Fence)
{
    FenceSync(Fence, FenceSignal(Fence));
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
	if (Type == D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV  || Type == D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER)
		HeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
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
    ID3D12DescriptorHeap_GetCPUDescriptorHandleForHeapStart(Heap->Heap, &CPU);
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
void D3D12GetWindowDimension(HWND Window, u32* Width, u32* Height)
{
    RECT ClientRect;
    GetClientRect(Window, &ClientRect);
    *Width = ClientRect.right - ClientRect.left;
    *Height = ClientRect.bottom - ClientRect.top;
}

void D3D12LoadDXGI()
{
    HMODULE Library = LoadLibraryA("dxgi.dll");
    if (Library)
    {
        CreateDXGIFactory = (create_dxgi_factory*)GetProcAddress(Library, "CreateDXGIFactory");
    }
}

void D3D12LoadD3D12()
{
    HMODULE Library = LoadLibraryA("d3d12.dll");
    if (Library)
    {
        D3D12CreateDevice = (d3d12_create_device*)GetProcAddress(Library, "D3D12CreateDevice");
        D3D12GetDebugInterface = (d3d12_get_debug_interface*)GetProcAddress(Library, "D3D12GetDebugInterface");
        D3D12SerializeRootSignature = (d3d12_serialize_root_signature*)GetProcAddress(Library, "D3D12SerializeRootSignature");
    }
}

void D3D12LoadD3DShader()
{
    HMODULE Library = LoadLibraryA("d3dcompiler_47.dll");
    if (Library)
    {
        D3DCompile = (d3d_compile*)GetProcAddress(Library, "D3DCompile");
    }
}

internal
void D3D12GetHardwareAdapter(IDXGIFactory3* Factory, IDXGIAdapter1** RetAdapter, bool32 RequestHighPerformanceAdapter)
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

void D3D12InitBuffer(u64 BufferSize, u64 BufferStride, gpu_buffer_usage Usage, gpu_buffer* Buffer)
{
    Buffer->BufferSize = BufferSize;
    Buffer->BufferStride = BufferStride;
    Buffer->Usage = Usage;
    
    D3D12_HEAP_PROPERTIES HeapProperties;
    ZeroMemory(&HeapProperties, sizeof(D3D12_HEAP_PROPERTIES));
    HeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
    HeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    HeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
    HeapProperties.CreationNodeMask = 0;
    HeapProperties.VisibleNodeMask = 0;
    
    D3D12_RESOURCE_DESC ResourceDesc;
    ZeroMemory(&ResourceDesc, sizeof(D3D12_RESOURCE_DESC));
    ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    ResourceDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
    ResourceDesc.Width = BufferSize;
    ResourceDesc.Height = 1;
    ResourceDesc.DepthOrArraySize = 1;
    ResourceDesc.MipLevels = 1;
    ResourceDesc.Format = DXGI_FORMAT_UNKNOWN;
    ResourceDesc.SampleDesc.Count = 1;
    ResourceDesc.SampleDesc.Quality = 0;
    ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    ResourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
    
    HRESULT Result = ID3D12Device_CreateCommittedResource(D3D12.Device, &HeapProperties, 0, &ResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, NULL, &IID_ID3D12Resource, (void**)&Buffer->Resource);
    Assert(SUCCEEDED(Result));
    
    switch (Usage)
    {
        case GpuBufferUsage_Vertex:
        {
            Buffer->VertexBufferView.BufferLocation = ID3D12Resource_GetGPUVirtualAddress(Buffer->Resource);
            Buffer->VertexBufferView.SizeInBytes = BufferSize;
            Buffer->VertexBufferView.StrideInBytes = BufferStride;
            break;
        }
        case GpuBufferUsage_Index:
        {
            Buffer->IndexBufferView.BufferLocation = ID3D12Resource_GetGPUVirtualAddress(Buffer->Resource);
            Buffer->IndexBufferView.SizeInBytes = BufferSize;
            Buffer->IndexBufferView.Format = DXGI_FORMAT_R32_UINT;
            break;
        }
        case GpuBufferUsage_Constant:
        {
            Buffer->ConstantBufferView.BufferLocation = ID3D12Resource_GetGPUVirtualAddress(Buffer->Resource);
            Buffer->ConstantBufferView.SizeInBytes = BufferSize;
			Buffer->HeapIndex = AllocDescriptor(&D3D12.ShaderHeap);

			ID3D12Device_CreateConstantBufferView(D3D12.Device, &Buffer->ConstantBufferView, DescriptorHeapCPU(&D3D12.ShaderHeap, Buffer->HeapIndex));
            break;
        }
    }
}

void D3D12UploadBuffer(u64 BufferSize, const void* Data, gpu_buffer* Buffer)
{
    void* Pointer;
    Assert(SUCCEEDED(ID3D12Resource_Map(Buffer->Resource, 0, NULL, &Pointer)));
    memcpy(Pointer, Data, BufferSize);
    ID3D12Resource_Unmap(Buffer->Resource, 0, NULL);
}

void D3D12FreeBuffer(gpu_buffer* Buffer)
{
	if (Buffer->Usage == GpuBufferUsage_Constant)
		FreeDescriptor(&D3D12.ShaderHeap, Buffer->HeapIndex);
    SafeRelease(Buffer->Resource);
}

void D3D12InitForward()
{
	D3D12_ROOT_PARAMETER Parameters[2];
	ZeroMemory(Parameters, sizeof(Parameters));

	D3D12_DESCRIPTOR_RANGE CameraRange;
	CameraRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	CameraRange.NumDescriptors = 1;
	CameraRange.BaseShaderRegister = 0;
	CameraRange.RegisterSpace = 1;
	CameraRange.OffsetInDescriptorsFromTableStart = 0;

	Parameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	Parameters[0].Constants.Num32BitValues = 16;
	Parameters[0].Constants.RegisterSpace = 0;
	Parameters[0].Constants.ShaderRegister = 0;
	Parameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	Parameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	Parameters[1].DescriptorTable.NumDescriptorRanges = 1;
	Parameters[1].DescriptorTable.pDescriptorRanges = &CameraRange;
	Parameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

    D3D12_ROOT_SIGNATURE_DESC RootDesc;
    ZeroMemory(&RootDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
	RootDesc.pParameters = Parameters;
	RootDesc.NumParameters = 2;
    RootDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
    
    ID3DBlob* Signature = NULL;
    ID3DBlob* Error = NULL;
    
    HRESULT Result = D3D12SerializeRootSignature(&RootDesc, D3D_ROOT_SIGNATURE_VERSION_1, &Signature, &Error);
    Assert(SUCCEEDED(Result));
    Result = ID3D12Device_CreateRootSignature(D3D12.Device, 0, Signature->lpVtbl->GetBufferPointer(Signature), Signature->lpVtbl->GetBufferSize(Signature), &IID_ID3D12RootSignature, (void**)&D3D12.ForwardPipeline.RootSignature);
    Assert(SUCCEEDED(Result));
    
    ID3DBlob* VertexShader = NULL;
    ID3DBlob* FragmentShader = NULL;
    
    buffer ShaderBuffer;
    PlatformState.ReadFile("shaders/forward.hlsl", &ShaderBuffer);
    
    Result = D3DCompile(ShaderBuffer.Data, ShaderBuffer.Size, NULL, NULL, NULL, "VSMain", "vs_5_1", 0, 0, &VertexShader, &Error);
    if (Error) OutputDebugStringA((char*)Error->lpVtbl->GetBufferPointer(Error));
    Assert(SUCCEEDED(Result));
    Result = D3DCompile(ShaderBuffer.Data, ShaderBuffer.Size, NULL, NULL, NULL, "PSMain", "ps_5_1", 0, 0, &FragmentShader, &Error);
    if (Error) OutputDebugStringA((char*)Error->lpVtbl->GetBufferPointer(Error));
    Assert(SUCCEEDED(Result));
    
    D3D12_INPUT_ELEMENT_DESC InputElementDescs[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
    };
    
    D3D12_GRAPHICS_PIPELINE_STATE_DESC PSODesc;
    ZeroMemory(&PSODesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
    PSODesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
    PSODesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
    PSODesc.RasterizerState.DepthClipEnable = false;
    PSODesc.RasterizerState.FrontCounterClockwise = false;
    PSODesc.RasterizerState.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
    PSODesc.RasterizerState.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
    PSODesc.RasterizerState.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
    PSODesc.RasterizerState.MultisampleEnable = false;
    PSODesc.RasterizerState.AntialiasedLineEnable = false;
    PSODesc.RasterizerState.ForcedSampleCount = 1;
    PSODesc.RasterizerState.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
    PSODesc.pRootSignature = D3D12.ForwardPipeline.RootSignature;
    PSODesc.VS.pShaderBytecode = VertexShader->lpVtbl->GetBufferPointer(VertexShader);
    PSODesc.VS.BytecodeLength = VertexShader->lpVtbl->GetBufferSize(VertexShader);
    PSODesc.PS.pShaderBytecode = FragmentShader->lpVtbl->GetBufferPointer(FragmentShader);
    PSODesc.PS.BytecodeLength = FragmentShader->lpVtbl->GetBufferSize(FragmentShader);
    PSODesc.InputLayout.NumElements = 2;
    PSODesc.InputLayout.pInputElementDescs = InputElementDescs;
    PSODesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    PSODesc.BlendState.AlphaToCoverageEnable = false;
    PSODesc.BlendState.IndependentBlendEnable = false;
    PSODesc.BlendState.RenderTarget[0].BlendEnable = false;
    PSODesc.BlendState.RenderTarget[0].LogicOpEnable = false;
    PSODesc.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
    PSODesc.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_ZERO;
    PSODesc.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
    PSODesc.BlendState.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
    PSODesc.BlendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
    PSODesc.BlendState.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
    PSODesc.BlendState.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
    PSODesc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
    PSODesc.SampleMask = UINT_MAX;
    PSODesc.NumRenderTargets = 1;
    PSODesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    PSODesc.SampleDesc.Count = 1;
    PSODesc.SampleDesc.Quality = 0;
    
    Result = ID3D12Device_CreateGraphicsPipelineState(D3D12.Device, &PSODesc, &IID_ID3D12PipelineState, (void**)&D3D12.ForwardPipeline.PipelineState);
    Assert(SUCCEEDED(Result));

	D3D12InitBuffer(Align256(sizeof(m4) * 2), 0, GpuBufferUsage_Constant, &D3D12.CameraBuffer);
    
    PlatformState.HeapFree(ShaderBuffer.Data);
    SafeRelease(VertexShader);
    SafeRelease(FragmentShader);
    SafeRelease(Signature);
    SafeRelease(Error);
}

void D3D12Init(HWND Window)
{
    D3D12.RenderWindow = Window;
    
    D3D12LoadDXGI();
    D3D12LoadD3D12();
    D3D12LoadD3DShader();
    
    HRESULT Result = D3D12GetDebugInterface(&IID_ID3D12Debug1, (void**)&D3D12.Debug);
    Assert(SUCCEEDED(Result));
    ID3D12Debug1_EnableDebugLayer(D3D12.Debug);
    
    Result = CreateDXGIFactory(&IID_IDXGIFactory, (void**)&D3D12.Factory);
    Assert(SUCCEEDED(Result));
    D3D12GetHardwareAdapter(D3D12.Factory, &D3D12.Adapter, true);
    
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
	DescriptorHeapInit(&D3D12.ShaderHeap, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1000000);
    
    // Create Swap Chain
    DXGI_SWAP_CHAIN_DESC1 SwapchainDesc = {0};
    D3D12GetWindowDimension(D3D12.RenderWindow, &SwapchainDesc.Width, &SwapchainDesc.Height);
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
    
    for (u32 BufferIndex = 0; BufferIndex < FRAMES_IN_FLIGHT; BufferIndex++)
    {
        // Get buffers
        IDXGISwapChain3_GetBuffer(D3D12.Swapchain, BufferIndex, &IID_ID3D12Resource, (void**)&D3D12.SwapchainBuffers[BufferIndex]);
        D3D12.SwapchainRenderTargets[BufferIndex] = AllocDescriptor(&D3D12.RenderTargetViewHeap);
        
        ID3D12Device_CreateRenderTargetView(D3D12.Device, D3D12.SwapchainBuffers[BufferIndex], NULL, DescriptorHeapCPU(&D3D12.RenderTargetViewHeap, D3D12.SwapchainRenderTargets[BufferIndex]));
        
        // Create command objects
        ID3D12Device_CreateCommandAllocator(D3D12.Device, D3D12_COMMAND_LIST_TYPE_DIRECT, &IID_ID3D12CommandAllocator, (void**)&D3D12.CommandAllocators[BufferIndex]);
        ID3D12Device_CreateCommandList(D3D12.Device, 0, D3D12_COMMAND_LIST_TYPE_DIRECT, D3D12.CommandAllocators[BufferIndex], NULL, &IID_ID3D12GraphicsCommandList, (void**)&D3D12.CommandLists[BufferIndex]);
        ID3D12GraphicsCommandList_Close(D3D12.CommandLists[BufferIndex]);
    }
    
    D3D12InitForward();
}

void D3D12Wait()
{
    FenceFlush(&D3D12.DeviceFence);
}

void D3D12Exit()
{
	D3D12FreeBuffer(&D3D12.CameraBuffer);
    SafeRelease(D3D12.ForwardPipeline.PipelineState);
    SafeRelease(D3D12.ForwardPipeline.RootSignature);
    
    for (u32 BufferIndex = 0; BufferIndex < FRAMES_IN_FLIGHT; BufferIndex++)
    {
        SafeRelease(D3D12.CommandAllocators[BufferIndex]);
        SafeRelease(D3D12.CommandLists[BufferIndex]);
        
        FreeDescriptor(&D3D12.RenderTargetViewHeap, D3D12.SwapchainRenderTargets[BufferIndex]);
        ID3D12Resource_Release(D3D12.SwapchainBuffers[BufferIndex]);
    }
    
    SafeRelease(D3D12.Swapchain);
	DescriptorHeapFree(&D3D12.ShaderHeap);
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

void D3D12PushTransform(m4 Transform)
{
	ID3D12GraphicsCommandList* Command = D3D12.CommandLists[D3D12.FrameIndex];
	ID3D12GraphicsCommandList_SetGraphicsRoot32BitConstants(Command, 0, 16, Transform.Elements, 0);
}

void D3D12Begin(renderer_begin* Begin)
{
	m4 Matrices[2] = { Begin->View, Begin->Projection };
	D3D12UploadBuffer(Align256(sizeof(Matrices)), Matrices, &D3D12.CameraBuffer);

    u32 Width = 0;
    u32 Height = 0;
    D3D12GetWindowDimension(D3D12.RenderWindow, &Width, &Height);
    
    if (Width == 0 || Height == 0)
    {
        return;
    }
    
    D3D12.FrameIndex = IDXGISwapChain3_GetCurrentBackBufferIndex(D3D12.Swapchain);
    FenceSync(&D3D12.DeviceFence, D3D12.FrameSync[D3D12.FrameIndex]);
    
    ID3D12CommandAllocator* Allocator = D3D12.CommandAllocators[D3D12.FrameIndex];
    ID3D12GraphicsCommandList* Command = D3D12.CommandLists[D3D12.FrameIndex];
    
    ID3D12CommandAllocator_Reset(Allocator);
    ID3D12GraphicsCommandList_Reset(Command, Allocator, NULL);
    
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
    ID3D12GraphicsCommandList_IASetPrimitiveTopology(Command, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    ID3D12GraphicsCommandList_SetGraphicsRootSignature(Command, D3D12.ForwardPipeline.RootSignature);
    ID3D12GraphicsCommandList_SetPipelineState(Command, D3D12.ForwardPipeline.PipelineState);

	ID3D12DescriptorHeap* const HeapsToBind[1] = { D3D12.ShaderHeap.Heap };

	ID3D12GraphicsCommandList_SetDescriptorHeaps(Command, 1, HeapsToBind);
	ID3D12GraphicsCommandList_SetGraphicsRootDescriptorTable(Command, 1, DescriptorHeapGPU(&D3D12.ShaderHeap, D3D12.CameraBuffer.HeapIndex));
}

void D3D12End()
{    
    u32 Width = 0;
    u32 Height = 0;
    D3D12GetWindowDimension(D3D12.RenderWindow, &Width, &Height);
    
    if (Width == 0 || Height == 0)
    {
        return;
    }
    
    ID3D12GraphicsCommandList* Command = D3D12.CommandLists[D3D12.FrameIndex];
    
    D3D12_RESOURCE_BARRIER Barrier;
    ZeroMemory(&Barrier, sizeof(D3D12_RESOURCE_BARRIER));
    Barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    Barrier.Transition.pResource = D3D12.SwapchainBuffers[D3D12.FrameIndex];
    Barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
    Barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
    Barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
    Barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
    ID3D12GraphicsCommandList_ResourceBarrier(Command, 1, &Barrier);
    
    ID3D12GraphicsCommandList_Close(Command);
    
    // Execute
    ID3D12CommandList* CommandLists[] = { (ID3D12CommandList*)Command };
    ID3D12CommandQueue_ExecuteCommandLists(D3D12.Queue, 1, CommandLists);
    
    // Present
    IDXGISwapChain3_Present(D3D12.Swapchain, true, 0);
    D3D12.FrameSync[D3D12.FrameIndex] = FenceSignal(&D3D12.DeviceFence);
}

void D3D12BindBuffer(gpu_buffer* Buffer)
{
    ID3D12GraphicsCommandList* Command = D3D12.CommandLists[D3D12.FrameIndex];
    
    switch (Buffer->Usage)
    {
        case GpuBufferUsage_Vertex:
        {
            const D3D12_VERTEX_BUFFER_VIEW Views[] = { Buffer->VertexBufferView }; 
            ID3D12GraphicsCommandList_IASetVertexBuffers(Command, 0, 1, Views);
            break;
        }
        case GpuBufferUsage_Index:
        {
            ID3D12GraphicsCommandList_IASetIndexBuffer(Command, &Buffer->IndexBufferView);
            break;
        }
    }
}

void D3D12Draw(u32 VertexCount)
{
    ID3D12GraphicsCommandList* Command = D3D12.CommandLists[D3D12.FrameIndex];
    ID3D12GraphicsCommandList_DrawInstanced(Command, VertexCount, 1, 0, 0);
}

void D3D12DrawIndexed(u32 IndexCount)
{
	ID3D12GraphicsCommandList* Command = D3D12.CommandLists[D3D12.FrameIndex];
	ID3D12GraphicsCommandList_DrawIndexedInstanced(Command, IndexCount, 1, 0, 0, 0);
}

void D3D12Resize(u32 Width, u32 Height)
{
    if (D3D12.Swapchain)
    {
        FenceFlush(&D3D12.DeviceFence);
        
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