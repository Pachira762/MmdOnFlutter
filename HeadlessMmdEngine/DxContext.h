#pragma once
#include <wrl.h>
#include "directx/d3d12.h"
#include "directx/d3dx12.h"
#include "DxContextBase.h"
#include "DxUtil.h"

namespace headless_mmd {

class DxContext : public DxContextBase {
public:
    virtual ~DxContext() = default;

    virtual bool Init(HWND hwnd) override;

private:
    using super = DxContextBase;

public:
    bool CopyResource(IDXResource* dest, IDXResource* src, D3D12_RESOURCE_STATES state = D3D12_RESOURCE_STATE_COMMON) {
        return Execute([dest, src, state](IDXGraphicsCommandList* command_list)->bool {
            command_list->CopyResource(dest, src);

            auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(dest, D3D12_RESOURCE_STATE_COPY_DEST, state);
            command_list->ResourceBarrier(1, &barrier);

            return true;
            });
    }

    IDXRootSignaturePtr CreateRootSignature(const D3D12_ROOT_PARAMETER1* parameters, int num_parameters, const D3D12_STATIC_SAMPLER_DESC* samplers = nullptr, int num_samplers = 0, D3D12_ROOT_SIGNATURE_FLAGS flags = D3D12_ROOT_SIGNATURE_FLAG_NONE) {
        D3D12_VERSIONED_ROOT_SIGNATURE_DESC desc{};
        desc.Version = D3D_ROOT_SIGNATURE_VERSION_1_1;
        desc.Desc_1_1.pParameters = parameters;
        desc.Desc_1_1.NumParameters = num_parameters;
        desc.Desc_1_1.pStaticSamplers = samplers;
        desc.Desc_1_1.NumStaticSamplers = num_samplers;
        desc.Desc_1_1.Flags = flags;

        IDXBlobPtr blob{}, error{};
        HRESULT hr = D3D12SerializeVersionedRootSignature(&desc, &blob, &error);
        if (FAILED(hr)) {
            if (error && error->GetBufferPointer()) OutputDebugStringA((const char*)error->GetBufferPointer());
            return {};
        }

        IDXRootSignaturePtr root_signature{};
        hr = core_->GetDevice()->CreateRootSignature(0, blob->GetBufferPointer(), blob->GetBufferSize(), IID_PPV_ARGS(&root_signature));
        if (FAILED(hr)) {
            return {};
        }

        return root_signature;
    }

    IDXPipelineStatePtr CreatePipelineState(const D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc) {
        IDXPipelineStatePtr pso{};
        core_->GetDevice()->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&pso));
        return pso;
    }

    IDXPipelineStatePtr CreatePipelineState(const D3D12_COMPUTE_PIPELINE_STATE_DESC& desc) {
        IDXPipelineStatePtr pso{};
        core_->GetDevice()->CreateComputePipelineState(&desc, IID_PPV_ARGS(&pso));
        return pso;
    }

    IDXResourcePtr CreateResource(const D3D12_HEAP_PROPERTIES& heap_props, D3D12_HEAP_FLAGS heap_flags, const D3D12_RESOURCE_DESC& desc, D3D12_RESOURCE_STATES initial_state, D3D12_CLEAR_VALUE* clear_value = nullptr) {
        IDXResourcePtr resource{};
        core_->GetDevice()->CreateCommittedResource(&heap_props, heap_flags, &desc, initial_state, clear_value, IID_PPV_ARGS(&resource));
        return resource;
    }

    IDXResourcePtr CreateBuffer(UINT64 size, const void* src, D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE, D3D12_RESOURCE_STATES initial_state = D3D12_RESOURCE_STATE_COMMON) {
        auto buffer = CreateResource(CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE,
            CD3DX12_RESOURCE_DESC::Buffer(size, flags), D3D12_RESOURCE_STATE_COPY_DEST);
        if (!buffer) {
            return {};
        }

        auto upload = CreateResource(CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), D3D12_HEAP_FLAG_NONE,
            CD3DX12_RESOURCE_DESC::Buffer(size, flags), D3D12_RESOURCE_STATE_GENERIC_READ);
        if (!upload) {
            return {};
        }

        void* dest{};
        if (FAILED(upload->Map(0, nullptr, &dest))) {
            return {};
        }

        memcpy_s(dest, size, src, size);
        upload->Unmap(0, nullptr);

        if (!CopyResource(buffer.Get(), upload.Get(), initial_state)) {
            return {};
        }

        return buffer;
    }

    IDXResourcePtr CreateDynamicBuffer(UINT64 size, D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE, D3D12_RESOURCE_STATES initial_state = D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_CLEAR_VALUE* clear_value = nullptr) {
        auto buffer = CreateResource(CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), D3D12_HEAP_FLAG_NONE,
            CD3DX12_RESOURCE_DESC::Buffer(size, flags), initial_state, clear_value);
        if (!buffer) {
            return {};
        }

        return buffer;
    }

    IDXResourcePtr CreateTexture(UINT width, UINT height, const void* src, UINT pitch = 0, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM, D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE, D3D12_RESOURCE_STATES initial_state = D3D12_RESOURCE_STATE_COMMON) {
        if (pitch == 0) {
            pitch = 4 * width;
        }

        auto texture = CreateResource(CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE,
            CD3DX12_RESOURCE_DESC::Tex2D(format, width, height, 1, 1, 1, 0, flags), D3D12_RESOURCE_STATE_COPY_DEST);
        if (!texture) {
            return {};
        }

        auto upload = CreateResource(CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0), D3D12_HEAP_FLAG_NONE,
            CD3DX12_RESOURCE_DESC::Tex2D(format, width, height, 1, 1, 1, 0, D3D12_RESOURCE_FLAG_NONE), D3D12_RESOURCE_STATE_GENERIC_READ);
        if (!upload) {
            return {};
        }

        if (FAILED(upload->WriteToSubresource(0, nullptr, src, pitch, pitch * height))) {
            return {};
        }

        if (!CopyResource(texture.Get(), upload.Get(), initial_state)) {
            return {};
        }

        return texture;
    }

    D3D12_GPU_DESCRIPTOR_HANDLE CreateConstantBufferView(D3D12_GPU_VIRTUAL_ADDRESS location, UINT size) {
        D3D12_CONSTANT_BUFFER_VIEW_DESC desc{};
        desc.BufferLocation = location;
        desc.SizeInBytes = size;
        return srv_heap_->CreateConstantBufferView(core_->GetDevice(), desc);
    }

    D3D12_GPU_DESCRIPTOR_HANDLE CreateShaderResourceView(IDXResource* resource, const D3D12_SHADER_RESOURCE_VIEW_DESC& desc) {
        return srv_heap_->CreateShaderResourceView(core_->GetDevice(), resource, desc);
    }

    D3D12_GPU_DESCRIPTOR_HANDLE CreateShaderResourceViewTex2D(IDXResource* resource, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM, UINT mip_levels = 1, UINT most_detailed_mip = 0) {
        D3D12_SHADER_RESOURCE_VIEW_DESC desc{};
        desc.Format = format;
        desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        desc.Texture2D.MostDetailedMip = most_detailed_mip;
        desc.Texture2D.MipLevels = mip_levels;
        return srv_heap_->CreateShaderResourceView(core_->GetDevice(), resource, desc);
    }
};

}