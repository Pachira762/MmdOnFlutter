#pragma once
#include <wrl.h>
#include <dxgi1_6.h>
#include "directx/d3d12.h"
#include "directx/d3dx12.h"

#define DEFINE_DX_CLASS(name, versioned_name) \
using name = versioned_name;\
using name##Ptr = Microsoft::WRL::ComPtr<versioned_name>;

namespace headless_mmd {

DEFINE_DX_CLASS(IDXBlob, ID3DBlob)
DEFINE_DX_CLASS(IDXFactory, IDXGIFactory7)
DEFINE_DX_CLASS(IDXSwapChain, IDXGISwapChain4)
DEFINE_DX_CLASS(IDXSwapChain1, IDXGISwapChain1)
DEFINE_DX_CLASS(IDXDebug, ID3D12Debug5)
DEFINE_DX_CLASS(IDXDevice, ID3D12Device9)
DEFINE_DX_CLASS(IDXCommandQueue, ID3D12CommandQueue)
DEFINE_DX_CLASS(IDXCommandAllocator, ID3D12CommandAllocator)
DEFINE_DX_CLASS(IDXCommandList, ID3D12CommandList)
DEFINE_DX_CLASS(IDXGraphicsCommandList, ID3D12GraphicsCommandList6)
DEFINE_DX_CLASS(IDXFence, ID3D12Fence1)
DEFINE_DX_CLASS(IDXResource, ID3D12Resource2)
DEFINE_DX_CLASS(IDXDescriptorHeap, ID3D12DescriptorHeap)
DEFINE_DX_CLASS(IDXRootSignature, ID3D12RootSignature)
DEFINE_DX_CLASS(IDXPipelineState, ID3D12PipelineState)

}

#undef DEFINE_DX_CLASS
