#include "pch.h"
#include "DescriptorHeap.h"
#include "DxUtil.h"

namespace headless_mmd {

bool DescriptorHeap::Init(IDXDevice* device, D3D12_DESCRIPTOR_HEAP_TYPE type, int num, D3D12_DESCRIPTOR_HEAP_FLAGS flags) {
	increment_size_ = device->GetDescriptorHandleIncrementSize(type);

	D3D12_DESCRIPTOR_HEAP_DESC desc{};
	desc.Type = type;
	desc.NumDescriptors = num;
	desc.Flags = flags;
	if (FAILED(device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&descriptor_heap_)))) {
		return false;
	}

	Reset();

	return true;
}

void DescriptorHeap::Reset() {
	if (!descriptor_heap_) {
		return;
	}

	cpu_handle_ = descriptor_heap_->GetCPUDescriptorHandleForHeapStart();

	if (descriptor_heap_->GetDesc().Flags & D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE) {
		gpu_handle_ = descriptor_heap_->GetGPUDescriptorHandleForHeapStart();
	}
}

D3D12_GPU_DESCRIPTOR_HANDLE DescriptorHeap::CreateConstantBufferView(IDXDevice* device, const D3D12_CONSTANT_BUFFER_VIEW_DESC& desc) {
	return CreateView(&IDXDevice::CreateConstantBufferView, device, &desc, cpu_handle_);
}

D3D12_GPU_DESCRIPTOR_HANDLE DescriptorHeap::CreateShaderResourceView(IDXDevice* device, IDXResource* resource, const D3D12_SHADER_RESOURCE_VIEW_DESC& desc) {
	return CreateView(&IDXDevice::CreateShaderResourceView, device, resource, &desc, cpu_handle_);
}

template<class Func, class... Args>
D3D12_GPU_DESCRIPTOR_HANDLE DescriptorHeap::CreateView(Func&& func, IDXDevice* device, Args&&... args) {
	auto ret = gpu_handle_;

	std::invoke(std::forward<Func>(func), device, std::forward<Args>(args)...);
	cpu_handle_.ptr += increment_size_;
	gpu_handle_.ptr += increment_size_;

	return ret;
}

}