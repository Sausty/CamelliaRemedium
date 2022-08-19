#ifndef CAMELLIA_D3D12_H
#define CAMELLIA_D3D12_H

#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3d12shader.h>
#include <Windows.Graphics.Holographic.Interop.h>

// NOTE(milo.h): I wrote this entire file by hand. I am close to insanity.

//~ NOTE(milo.h): ID3D12CommandAllocator
#define ID3D12CommandAllocator_Reset(c) c->lpVtbl->Reset(c)
#define ID3D12CommandAllocator_GetPrivateData(c, guid, pDataSize, pData) c->lpVtbl->GetPrivateData(c, guid, pDataSize, pData)
#define ID3D12CommandAllocator_SetName(c, Name) c->lpVtbl->SetName(c, Name)
#define ID3D12CommandAllocator_SetPrivateData(c, guid, DataSize, pData) c->lpVtbl->SetPrivateData(c, guid, DataSize, pData)
#define ID3D12CommandAllocator_SetPrivateDataInterface(c, guid, pData) c->lpVtbl->SetPrivateDataInterface(c, guid, pData)
#define ID3D12CommandAllocator_AddRef(c) c->lpVtbl->AddRef(c)
#define ID3D12CommandAllocator_QueryInterface(c, riid, ppvObject) c->lpVtbl->QueryInterface(c, riid, ppvObject)
#define ID3D12CommandAllocator_Release(c) c->lpVtbl->Release(c)
#define ID3D12CommandAllocator_GetDevice(c, riid, ppvDevice) c->lpVtbl->GetDevice(c, riid, ppvDevice)

//~ NOTE(milo.h): ID3D12CommandList
#define ID3D12CommandList_GetType(c) c->lpVtbl->GetType(c)
#define ID3D12CommandList_GetPrivateData(c, guid, pDataSize, pData) c->lpVtbl->GetPrivateData(c, guid, pDataSize, pData)
#define ID3D12CommandList_SetName(c, Name) c->lpVtbl->SetName(c, Name)
#define ID3D12CommandList_SetPrivateData(c, guid, DataSize, pData) c->lpVtbl->SetPrivateData(c, guid, DataSize, pData)
#define ID3D12CommandList_SetPrivateDataInterface(c, guid, pData) c->lpVtbl->SetPrivateDataInterface(c, guid, pData)
#define ID3D12CommandList_AddRef(c) c->lpVtbl->AddRef(c)
#define ID3D12CommandList_QueryInterface(c, riid, ppvObject) c->lpVtbl->QueryInterface(c, riid, ppvObject)
#define ID3D12CommandList_Release(c) c->lpVtbl->Release(c)
#define ID3D12CommandList_GetDevice(c, riid, ppvDevice) c->lpVtbl->GetDevice(c, riid, ppvDevice)

//~ NOTE(milo.h): ID3D12CommandQueue
#define ID3D12CommandQueue_BeginEvent(c, Metadata, pData, Size) c->lpVtbl->BeginEvent(c, Metadata, pData, Size)
#define ID3D12CommandQueue_CopyTileMappings(c, pDstResource, pDstRegionStartCoordinate, pSrcResource, pSrcRegionStartCoordinatepRegionSize, Flags) c->lpVtbl->CopyTileMappings(c, pDstResource, pDstRegionStartCoordinate, pSrcResource, pSrcRegionStartCoordinate)
#define ID3D12CommandQueue_EndEvent(c) c->lpVtbl->EndEvent(c)
#define ID3D12CommandQueue_ExecuteCommandLists(c, NumCommandLists, ppCommandLists) c->lpVtbl->ExecuteCommandLists(c, NumCommandLists, ppCommandLists)
#define ID3D12CommandQueue_GetClockCalibration(c, pGpuTimestamp, pCpuTimestamp) c->lpVtbl->GetClockCalibration(c, pGpuTimestamp, pCpuTimestamp)
#define ID3D12CommandQueue_GetDesc(c) c->lpVtbl->GetDesc(c)
#define ID3D12CommandQueue_GetTimestampFrequency(c, pFrequency) c->lpVtbl->GetTimestampFrequency(c, pFrequency)
#define ID3D12CommandQueue_SetMarker(c, Metadata, pData, Size) c->lpVtbl->SetMarker(c, Metadata, pData, Size)
#define ID3D12CommandQueue_Signal(c, pFence, Value) c->lpVtbl->Signal(c, pFence, Value)
#define ID3D12CommandQueue_UpdateTileMappings(c, pResource, NumResourceRegions, pResourceRegionStartCoordinates, pResourceRegionSizes, pHeap, NumRanges, pRangeFlags, pHeapRangeStartOffsets, pRangeTileCounts, Flags) c->lpVtbl->UpdateTileMappings(c, pResource, NumResourceRegions, pResourceRegionStartCoordinates, pResourceRegionSizes, pHeap, NumRanges, pRangeFlags, pHeapRangeStartOffsets, pRangeTileCounts, Flags)
#define ID3D12CommandQueue_Wait(c, pFence, Value) c->lpVtbl->Wait(c, pFence, Value)
#define ID3D12CommandQueue_GetPrivateData(c, guid, pDataSize, pData) c->lpVtbl->GetPrivateData(c, guid, pDataSize, pData)
#define ID3D12CommandQueue_SetName(c, Name) c->lpVtbl->SetName(c, Name)
#define ID3D12CommandQueue_SetPrivateData(c, guid, DataSize, pData) c->lpVtbl->SetPrivateData(c, guid, DataSize, pData)
#define ID3D12CommandQueue_SetPrivateDataInterface(c, guid, pData) c->lpVtbl->SetPrivateDataInterface(c, guid, pData)
#define ID3D12CommandQueue_AddRef(c) c->lpVtbl->AddRef(c)
#define ID3D12CommandQueue_QueryInterface(c, riid, ppvObject) c->lpVtbl->QueryInterface(c, riid, ppvObject)
#define ID3D12CommandQueue_Release(c) c->lpVtbl->Release(c)
#define ID3D12CommandQueue_GetDevice(c, riid, ppvDevice) c->lpVtbl->GetDevice(c, riid, ppvDevice)

//~ NOTE(milo.h): ID3D12DescriptorHeap
#define ID3D12DescriptorHeap_GetCPUDescriptorHeapHandleForHeapStart(d, RetVal) d->lpVtbl->GetCPUDescriptorHandleForHeapStart(d, RetVal)
#define ID3D12DescriptorHeap_GetDesc(d) d->lpVtbl->GetDesc(d)
#define ID3D12DescriptorHeap_GetGPUDescriptorHandleForHeapStart(d, RetVal) d->lpVtbl->GetGPUDescriptorHandleForHeapStart(d, RetVal)
#define ID3D12DescriptorHeap_GetPrivateData(d, guid, pDataSize, pData) d->lpVtbl->GetPrivateData(d, guid, pDataSize, pData)
#define ID3D12DescriptorHeap_SetName(d, Name) d->lpVtbl->SetName(d, Name)
#define ID3D12DescriptorHeap_SetPrivateData(d, guid, DataSize, pData) d->lpVtbl->SetPrivateData(d, guid, DataSize, pData)
#define ID3D12DescriptorHeap_SetPrivateDataInterface(d, guid, pData) d->lpVtbl->SetPrivateDataInterface(d, guid, pData)
#define ID3D12DescriptorHeap_AddRef(d) d->lpVtbl->AddRef(d)
#define ID3D12DescriptorHeap_QueryInterface(d, riid, ppvObject) d->lpVtbl->QueryInterface(d, riid, ppvObject)
#define ID3D12DescriptorHeap_Release(d) d->lpVtbl->Release(d)
#define ID3D12DescriptorHeap_GetDevice(d, riid, ppvDevice) d->lpVtbl->GetDevice(c, riid, ppvDevice)

//~ NOTE(milo.h): ID3D12Device
#define ID3D12Device_CheckFeatureSupport(d, Feature, pFeatureSupportData, FeatureSupportDataSize) d->lpVtbl->CheckFeatureSupport(d, Feature, pFeatureSupportData, FeatureSupportDataSize)
#define ID3D12Device_CopyDescriptors(d, NumDestDescriptorRanges, pDestDescriptorRangeStarts, pDestDescriptorRangeSizes, NumSrcDescriptorRanges, pSrcDescriptorRangeStarts, pSrcDescriptorRangeSizes, DescriptorHeapsType) d->lpVtbl->CopyDescriptors(d, NumDescriptorRanges, pDestDescriptorRangeStarts, NumSrcDescriptorRanges, pSrcDescriptorRangeStarts, pSrcDescriptorRangeSizes, DescriptorHeapsType)
#define ID3D12Device_CopyDescriptorsSimple(d, NumDescriptors, DestDescriptorRangeStart, SrcDescriptorRangeStart, DescriptorHeapsType) d->lpVtbl->CopyDescriptorsSimple(d, DestDescriptorRangeStart, SrcDescriptorRangeStart, DescriptorHeapsType)
#define ID3D12Device_CreateCommandAllocator(d, type, riid, ppCommandAllocator) d->lpVtbl->CreateCommandAllocator(d, type, riid, ppCommandAllocator)
#define ID3D12Device_CreateCommandList(d, NodeMask, type, pCommandAllocator, pInitialState, riid, ppCommandList) d->lpVtbl->CreateCommandList(d, NodeMask, type, pCommandAllocator, pInitialState, riid, ppCommandList)
#define ID3D12Device_CreateCommandQueue(d, pDesc, riid, ppCommandQueue) d->lpVtbl->CreateCommandQueue(d, pDesc, riid, ppCommandQueue)
#define ID3D12Device_CreateCommandSignature(d, pDesc, pRootSignature, riid, ppvCommandSignature) d->lpVtbl->CreateCommandSignature(d, pDesc, pRootSignature, riid, ppvCommandSignature)
#define ID3D12Device_CreateCommittedResource(d, pHeapProperties, HeapFlags, pDesc, InitialResourceState, pOptimizedClearValue, riidResource, ppvResource) d->lpVtbl->CreateCommittedResource(d, pHeapProperties, HeapFlags, pDesc, InitialResourceState, pOptimizedClearValue, riidResource, ppvResource)
#define ID3D12Device_CreateComputePipelineState(d, pDesc, riid, ppPipelineState) d->lpVtbl->CreateComputePipelineState(d, pDesc, riid, ppPipelineState)
#define ID3D12Device_CreateConstantBufferView(d, pDesc, DestDescriptor) d->lpVtbl->CreateConstantBufferView(d, pDesc, DestDescriptor)
#define ID3D12Device_CreateDepthStencilView(d, pResource, pDesc, DestDescriptor) d->lpVtbl->CreateDepthStencilView(d, pResource, pDesc, DestDescriptor)
#define ID3D12Device_CreateDescriptorHeap(d, pDescriptorHeapDesc, riid, ppvHeap) d->lpVtbl->CreateDescriptorHeap(d, pDescriptorHeapDesc, riid, ppvHeap)
#define ID3D12Device_CreateFence(d, InitialValue, Flags, riid, ppFence) d->lpVtbl->CreateFence(d, InitialValue, Flags, riid, ppFence)
#define ID3D12Device_CreateGraphicsPipelineState(d, pDesc, riid, ppPipelineState) d->lpVtbl->CreateGraphicsPipelineState(d, pDesc, riid, ppPipelineState)
#define ID3D12Device_CreateHeap(d, pDesc, riid, ppvHeap) d->lpVtbl->CreateHeap(d, pDesc, riid, ppvHeap)
#define ID3D12Device_CreatePlacedResource(d, pHeap, HeapOffset, pDesc, InitialState, pOptimizedClearValue, riid, ppvResource) d->lpVtbl->CreatePlacedResource(d, pHeap, HeapOffset, pDesc, InitialState, pOptimizedClearValue, riid, ppvResource)
#define ID3D12Device_CreateQueryHeap(d, pDesc, riid, ppvHeap) d->lpVtbl->CreateQueryHeap(d, pDesc, riid, ppvHeap)
#define ID3D12Device_CreateRenderTargetView(d, pResource, pDesc, DestDescriptor) d->lpVtbl->CreateRenderTargetView(d, pResource, pDesc, DestDescriptor)
#define ID3D12Device_CreateReservedResource(d, pDesc, InitialState, pOptimizedClearValue, riid, ppvResource) d->lpVtbl->CreateReservedResource(d, pDesc, InitialState, pOptimizedClearValue, riid, ppvResource)
#define ID3D12Device_CreateRootSignature(d, NodeMask, pBlobWithRootSignature, blobLengthInBytes, riid, ppvRootSignature) d->lpVtbl->CreateRootSignature(d, NodeMask, pBlobWithRootSignature, blobLengthInBytes, riid, ppvRootSignature)
#define ID3D12Device_CreateSampler(d, pDesc, DestDescriptor) d->lpVtbl->CreateSampler(d, pDesc, DestDescriptor)
#define ID3D12Device_CreateShaderResourceView(d, pResource, pDesc, DestDescriptor) d->lpVtbl->CreateShaderResourceView(d, pResource, pDesc, DestDescriptor)
#define ID3D12Device_CreateSharedHandle(d, pObject, pAttributes, Access, Name, pHandle) d->lpVtbl->CreateSharedHandle(d, pObject, pAttributes, Access, Name, pHandle)
#define ID3D12Device_CreateUnorderedAccessView(d, pResource, pCounterResource, pDesc, DestDescriptor) d->lpVtbl->CreateUnorderedAccessView(d, pResource, pCounterResource, pDesc, DestDescriptor)
#define ID3D12Device_Evict(d, NumObjets, ppObjects) d->lpVtbl->Evict(d, NumObjects, ppObjects)
#define ID3D12Device_GetAdapterLuid(d, RetVal) d->lpVtbl->GetAdapterLuid(d, RetVal)
#define ID3D12Device_GetCopyableFootprints(d, pResourceDesc, FirstSubresource, NumSubresources, BaseOffset, pLayouts, pNumRows, pRowSizeInBytes, pTotalBytes) d->lpVtbl->GetCopyableFootprints(d, pResourceDesc, FirstSubresource, NumSubresources, BaseOffset, pLayouts, pNumRows, pRowSizeInBytes, pTotalBytes)
#define ID3D12Device_GetCustomHeapProperties(d, RetVal, NodeMask, HeapType) d->lpVtbl->GetCustomHeapProperties(d, RetVal, NodeMask, HeapType)
#define ID3D12Device_GetDescriptorHandleIncrementSize(d, DescriptorHeapType) d->lpVtbl->GetDescriptorHandleIncrementSize(d, DescriptorHeapType)
#define ID3D12Device_GetDeviceRemovedReason(d) d->lpVtbl->GetDeviceRemovedReason(d)
#define ID3D12Device_GetNodeCount(d) d->lpVtbl->GetNodeCount(d)
#define ID3D12Device_GetResourceAllocationInfo(d, RetVal, visibleMask, numResourceDescs, pResourceDescs) d->lpVtbl->GetResourceAllocationInfo(d, RetVal, visibleMask, numResourceDescs, pResourceDescs)
#define ID3D12Device_GetResourceTiling(d, pTiledResource, pNumTilesForEntireResource, pPackedMipDesc, pStandardTileShapeForNonPackedMips, pNumSubresourceTilings, FirstSubresourceTilingToGet, pSubresourceTilingsForNonPackedMips) d->lpVtbl->GetResourceTiling(d, pTiledResource, pNumTilesForEntireResource, pPackedMipDesc, pStandardTileShapeForNonPackedMips, pNumSubresourceTilings, FirstSubresourceTilingToGet, pSubresourceTilingsForNonPackedMips)
#define ID3D12Device_MakeResidents(d, NumObjects, ppObjects) d->lpVtbl->MakeResidents(d, NumObjects, ppObjects)
#define ID3D12Device_OpenSharedHandle(d, NTHandle, riid, ppvObj) d->lpVtbl->OpenSharedHandle(d, NTHandle, riid, ppvObj)
#define ID3D12Device_OpenSharedHandleByName(d, Name, Access, pNTHandle) d->lpVtbl->OpenSharedHandleByName(d, Name, Access, pNTHandle)
#define ID3D12Device_SetStablePowerState(d, Enable) d->lpVtbl->SetStablePowerState(d, Enable)
#define ID3D12Device_GetPrivateData(d, guid, pDataSize, pData) d->lpVtbl->GetPrivateData(d, guid, pDataSize, pData)
#define ID3D12Device_SetName(d, Name) d->lpVtbl->SetName(d, Name)
#define ID3D12Device_SetPrivateData(d, guid, DataSize, pData) d->lpVtbl->SetPrivateData(d, guid, DataSize, pData)
#define ID3D12Device_SetPrivateDataInterface(d, guid, pData) d->lpVtbl->SetPrivateDataInterface(d, guid, pData)
#define ID3D12Device_AddRef(d) d->lpVtbl->AddRef(d)
#define ID3D12Device_QueryInterface(d, riid, ppvObject) d->lpVtbl->QueryInterface(d, riid, ppvObject)
#define ID3D12Device_Release(d) d->lpVtbl->Release(d)


//~ NOTE(milo.h): ID3D12Resource
#define ID3D12Resource_Release(Resource) Resource->lpVtbl->Release(Resource)

//~ NOTE(milo.h): ID3D12Fence
#define ID3D12Fence_Signal(Fence, FenceValue) Fence->lpVtbl->Signal(Fence, FenceValue)
#define ID3D12Fence_GetCompletedValue(Fence) Fence->lpVtbl->GetCompletedValue(Fence)
#define ID3D12Fence_SetEventOnCompletion(Fence, Value, Event) Fence->lpVtbl->SetEventOnCompletion(Fence, Value, Event)
/* D3D12 */
// TODO(milo.h): ID3D12DeviceRemovedExtendedData(IUnknown)
// TODO(milo.h): ID3D12DeviceRemovedExtendedDataSettings(IUnknown)
// TODO(milo.h): ID3D12Fence1(ID3D12Fence)
// TODO(milo.h): ID3D12GraphicsCommandList(ID3D12CommandList)
// TODO(milo.h): ID3D12GraphicsCommandList1(ID3D12GraphicsCommandList)
// TODO(milo.h): ID3D12GraphicsCommandList2(ID3D12GraphicsCommandList1)
// TODO(milo.h): ID3D12GraphicsCommandList3(ID3D12GraphicsCommandList2)
// TODO(milo.h): ID3D12GraphicsCommandList4(ID3D12GraphicsCommandList3)
// TODO(milo.h): ID3D12GraphicsCommandList5(ID3D12GraphicsCommandList4)
// TODO(milo.h): ID3D12Heap(ID3D12Pageable)
// TODO(milo.h); ID3D12LifetimeOwner(IUnknown)
// TODO(milo.h): ID3D12LifetimeTracker(ID3D12Pageable)
// TODO(milo.h): ID3D12MetaCommand(ID3D12Pageable)
// TODO(milo.h): ID3D12PipelineLibrary(ID3D12DeviceChild)
// TODO(milo.h): ID3D12PipelineLibrary1(ID3D12PipelineLibrary1)
// TODO(milo.h): ID3D12PipelineState(ID3D12Pageable)
// TODO(milo.h): ID3D12ProtectedResourceSession(ID3D12ProtectedSession)
// TODO(milo.h): ID3D12ProtectedResourceSession1(ID3D12ProtectedResourceSession)
// TODO(milo.h): ID3D12ProtectedSession(ID3D12DeviceChild)
// TODO(milo.h): ID3D12RootSignatureDeserializer(IUnknown)
// TODO(milo.h): ID3D12SDKConfiguration(IUnknown)
// TODO(milo.h): ID3D12ShaderCacheSession(ID3D12DeviceChild)
// TODO(milo.h): ID3D12StateObject(ID3D12Pageable)
// TODO(milo.h): ID3D12StateObjectProperties(IUnknown)
// TODO(milo.h): ID3D12Tools(IUnknown)
// TODO(milo.h): ID3D12VersionedRootSignatureDeserializer(IUnknown)
/* D3D12 */

//~ NOTE(milo.h): ID3D12Debug1
#define ID3D12Debug1_EnableDebugLayer(Debug) Debug->lpVtbl->EnableDebugLayer(Debug)
#define ID3D12Debug1_Release(Debug) Debug->lpVtbl->Release(Debug)

//~ NOTE(milo.h): ID3D12InfoQueue
#define ID3D12InfoQueue_SetBreakOnSeverity(InfoQueue, MessageSeverity, Enable) InfoQueue->lpVtbl->SetBreakOnSeverity(InfoQueue, MessageSeverity, Enable)
#define ID3D12InfoQueue_PushStorageFilter(InfoQueue, Filter) InfoQueue->lpVtbl->PushStorageFilter(InfoQueue, Filter)
#define ID3D12InfoQueue_Release(InfoQueue) InfoQueue->lpVtbl->Release(InfoQueue)

//~ NOTE(milo.h): ID3D12DebugDevice
#define ID3D12DebugDevice_ReportLiveDeviceObjects(DebugDevice, Flags) DebugDevice->lpVtbl->ReportLiveDeviceObjects(DebugDevice, Flags)

/* D3D12 DEBUG */
// TODO(milo.h): ID3D12Debug(IUnknown)
// TODO(milo.h): ID3D12Debug2(ID3D12Debug1)
// TODO(milo.h): ID3D12Debug3(ID3D12Debug2)
// TODO(milo.h): ID3D12Debug4(ID3D12Debug3)
// TODO(milo.h): ID3D12Debug5(ID3D12Debug4)
// TODO(milo.h): ID3D12DebugCommandList(IUnknown)
// TODO(milo.h): ID3D12DebugCommandList1(IUnknown)
// TODO(milo.h): ID3D12DebugCommandQueue(IUnknown)
// TODO(milo.h): ID3D12DebugDevice(IUnknown)
// TODO(milo.h): ID3D12DebugDevice1(IUnknown)
// TODO(milo.h): ID3D12SharingContract(IUnknown)
/* D3D12 DEBUG */

/* D3D12 SHADER */
// TODO(milo.h): ID3D12FunctionParameterReflection
// TODO(milo.h): ID3D12FunctionReflection
// TODO(milo.h): ID3D12LibraryReflection
// TODO(milo.h): ID3D12ShaderReflection
// TODO(milo.h): ID3D12ShaderReflectionConstantBuffer
// TODO(milo.h): ID3D12ShaderReflectionType
// TODO(milo.h): ID3D12ShaderReflectionVariable
/* D3D12 SHADER */

/* D3D12 Holographic Interop */
// TODO(milo.h): IHolographicCameraInterop(IInspectable)
// TODO(milo.h): IHolographicCameraRenderingParametersInterop(IInspectable)
// TODO(milo.h): IHolographicQuadLayerInterop(IInspectable)
// TODO(milo.h): IHolographicQuadLayerUpdateParametersInterop(IInspectable)
/* D3D12 Holographic Interop */

/* D3D11 on 12 */
// TODO(milo.h): ID3D11On12Device(IUnknown)
// TODO(milo.h): ID3D11On12Device1(ID3D11On12Device)
// TODO(milo.h): ID3D11On12Device2(ID3D11On12Device1)
/* D3D12 on 12 */

//~ NOTE(milo.h): IDXGIFactory3
#define IDXGIFactory3_EnumAdapters(Factory, Index, Adapter) Factory->lpVtbl->EnumAdapters(Factory, Index, Adapter)
#define IDXGIFactory3_CreateSwapChainForHwnd(Factory, pUnknown, Hwnd, pDesc, idk, idk1, ppvSwapChain) Factory->lpVtbl->CreateSwapChainForHwnd(Factory, pUnknown, Hwnd, pDesc, idk, idk1, ppvSwapChain)
#define IDXGIFactory3_QueryInterface(Factory, riid, ppvInterface) Factory->lpVtbl->QueryInterface(Factory, riid, ppvInterface)
#define IDXGIFactory3_EnumAdapters1(Factory, AdapterIndex, Adapter) Factory->lpVtbl->EnumAdapters1(Factory, AdapterIndex, Adapter)

//~ NOTE(milo.h): IDXGIFactory6
#define IDXGIFactory6_EnumAdapterByGpuPreference(Factory, Index, Preference, riid, ppvInterface) Factory->lpVtbl->EnumAdapterByGpuPreference(Factory, Index, Preference, riid, ppvInterface)

//~ NOTE(milo.h): IDXGIAdapter
#define IDXGIAdapter_GetParent(Adapter, riid, ppvParent) Adapter->lpVtbl->GetParent(Adapter, riid, ppvParent)

//~ NOTE(milo.h) IDXGIAdapter1
#define IDXGIAdapter1_GetDesc1(Adapter, pDesc) Adapter->lpVtbl->GetDesc1(Adapter, pDesc)

//~ NOTE(milo.h) IDXGISwapChain1
#define IDXGISwapChain1_QueryInterface(SwapChain, riid, ppvInterface) SwapChain->lpVtbl->QueryInterface(SwapChain, riid, ppvInterface)
#define IDXGISwapChain1_Release(SwapChain) SwapChain->lpVtbl->Release(SwapChain)

//~ NOTE(milo.h): IDXGISwapChain3
#define IDXGISwapChain3_GetBuffer(SwapChain, BufferIndex, riid, ppvBuffer) SwapChain->lpVtbl->GetBuffer(SwapChain, BufferIndex, riid, ppvBuffer)
#define IDXGISwapChain3_GetDesc1(SwapChain, Desc) SwapChain->lpVtbl->GetDesc1(SwapChain, Desc)
#define IDXGISwapChain3_ResizeBuffers(SwapChain, BufferCount, Width, Height, Format, Flags) SwapChain->lpVtbl->ResizeBuffers(SwapChain, BufferCount, Width, Height, Format, Flags)
#define IDXGISwapChain3_GetCurrentBackBufferIndex(SwapChain) SwapChain->lpVtbl->GetCurrentBackBufferIndex(SwapChain)
#define IDXGISwapChain3_Present(SwapChain, SyncInterval, Flags) SwapChain->lpVtbl->Present(SwapChain, SyncInterval, Flags)

/* DXGI */
// TODO(milo.h): IDXGIAdapter(IDXGIObject)
// TODO(milo.h): IDXGIAdapter1(IDXGIAdapter)
// TODO(milo.h): IDXGIDevice(IDXGIObject)
// TODO(milo.h): IDXGIDevice1(IDXGIDevice)
// TODO(milo.h): IDXGIDeviceSubObject(IDXGIObject)
// TODO(milo.h): IDXGIFactory(IDXGIObject)
// TODO(milo.h): IDXGIFactory1(IDXGIFactory)
// TODO(milo.h): IDXGIKeyedMutex(IDXGIDeviceSubObject)
// TODO(milo.h): IDXGIObject(IUnknown)
// TODO(milo.h): IDXGIOutput(IDXGIObject)
// TODO(milo.h): IDXGIResource(IDXGIDeviceSubObject)
// TODO(milo.h): IDXGISurface(IDXGIDeviceSubObject)
// TODO(milo.h): IDXGISurface1(IDXGISurface)
// TODO(milo.h): IDXGISwapChain(IDXGIDeviceSubObject)
// TODO(milo.h): IDXGIAdapter2(IDXGIAdapter1)
// TODO(milo.h): IDXGIDevice2(IDXGIDevice1)
// TODO(milo.h): IDXGIDisplayControl(IUnknown)
// TODO(milo.h): IDXGIFactory2(IDXGIFactory1)
// TODO(milo.h): IDXGIOutput1(IDXGIOutput)
// TODO(milo.h): IDXGIOutputDuplication(IDXGIObject)
// TODO(milo.h): IDXGIResource1(IDXGIResource)
// TODO(milo.h): IDXGISurface2(IDXGISurface)
// TODO(milo.h): IDXGISwapChain1(IDXGISwapChain)
// TODO(milo.h): IDXGIDecodeSwapChain(IUnknwon)
// TODO(milo.h): IDXGIDevice3(IDXGIDevice2)
// TODO(milo.h): IDXGIFactory3(IDXGIFactory2)
// TODO(milo.h): IDXGIFactoryMedia(IUnknown)
// TODO(milo.h): IDXGIOutput2(IDXGIOutput1)
// TODO(milo.h): IDXGIOutput3(IDXGIOutput2)
// TODO(milo.h): IDXGISwapChain2(IDXGISwapChain1)
// TODO(milo.h): IDXGISwapChainMedia(IUnknown)
// TODO(milo.h): IDXGIAdapter3(IDXGIAdapter2)
// TODO(milo.h): IDXGIFactory4(IDXGIFactory3)
// TODO(milo.h): IDXGIOutput4(IDXGIOutput3)
// TODO(milo.h): IDXGISwapChain3(IDXGISwapChain2)
// TODO(milo.h): IDXGIDevice4(IDXGIDevice3)
// TODO(milo.h): IDXGIFactory5(IDXGIFactory4)
// TODO(milo.h): IDXGIOutput5(IDXGIOutput4)
// TODO(milo.h): IDXGISwapChain4(IDXGISwapChain3)
// TODO(milo.h): IDXGIAdapter4(IDXGIAdapter3)
// TODO(milo.h): IDXGIFactory7(IDXGIFactory6)
// TODO(milo.h): IDXGIOutput6(IDXGIOutput5)
// TODO(milo.h): IDXGIDebug(IUnknown)
// TODO(milo.h): IDXGIDebug1(IDXGIDebug)
// TODO(milo.h): IDXGIInfoQueue(IUnknown)
/* DXGI */

#endif