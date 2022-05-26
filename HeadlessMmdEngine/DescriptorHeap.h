#pragma once
#include "DxClasses.h"

namespace headless_mmd {

class DescriptorHeap {
public:
	bool Init(IDXDevice* device, D3D12_DESCRIPTOR_HEAP_TYPE type, int num, D3D12_DESCRIPTOR_HEAP_FLAGS flags);
	void Reset();

	D3D12_GPU_DESCRIPTOR_HANDLE CreateConstantBufferView(IDXDevice* device, const D3D12_CONSTANT_BUFFER_VIEW_DESC& desc);
	D3D12_GPU_DESCRIPTOR_HANDLE CreateShaderResourceView(IDXDevice* device, IDXResource* resource, const D3D12_SHADER_RESOURCE_VIEW_DESC& desc);

private:
	template<class Func, class... Args>
	D3D12_GPU_DESCRIPTOR_HANDLE CreateView(Func&& func, IDXDevice* device, Args&&... args);

	IDXDescriptorHeapPtr	descriptor_heap_{};
	D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle_ = {};
	D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle_ = {};
	int increment_size_{};

public:
	IDXDescriptorHeap* GetDescriptorHeap() const {
		return descriptor_heap_.Get();
	}

	D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHandle() const {
		return cpu_handle_;
	}

	D3D12_GPU_DESCRIPTOR_HANDLE GetGpuHandle() const {
		return gpu_handle_;
	}
};

}