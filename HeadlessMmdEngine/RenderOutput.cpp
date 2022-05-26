#include "pch.h"
#include "RenderOutput.h"
#include "Common.h"
#include "DxUtil.h"

namespace {

SIZE GetClientSize(HWND hwnd) {
	RECT rc{};
	GetClientRect(hwnd, &rc);
	return { rc.right - rc.left, rc.bottom - rc.top };
}

}

namespace headless_mmd {

bool RenderOutput::Init(IDXDevice* device, IDXCommandQueue* command_queue, HWND hwnd, D3D12_CPU_DESCRIPTOR_HANDLE rtv_handle, D3D12_CPU_DESCRIPTOR_HANDLE dsv_handle) {
	const auto [width, height] = GetClientSize(hwnd);
	hwnd_ = hwnd;
	width_ = width;
	height_ = height;

	for (int i = 0; i < NumBackBuffers; ++i) {
		render_target_descriptor_[i] = rtv_handle;
		render_target_descriptor_[i].ptr += i * device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}
	depth_stencil_descriptor_ = dsv_handle;

	{
		auto hr = CreateDXGIFactory1(IID_PPV_ARGS(&factory_));
		if (FAILED(hr)) {
			ELOG(L"Failed to create DXGI Factory");
			return false;
		}

		IDXSwapChain1Ptr swap_chain1{};
		DXGI_SWAP_CHAIN_DESC1 desc{};
		desc.Width = width;
		desc.Height = height;
		desc.Format = RenderTargetFormat;
		desc.SampleDesc.Count = 1;
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.BufferCount = NumBackBuffers;
		desc.Scaling = DXGI_SCALING_STRETCH;
		desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		desc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
		desc.Flags = 0;
		hr = factory_->CreateSwapChainForHwnd(command_queue, hwnd, &desc, nullptr, nullptr, &swap_chain1);
		if (FAILED(hr)) {
			ELOG(L"Failed to create SwapChain for HWND:{:#X} size:{}, {} HRESULT:{:#X}", (std::uintptr_t)hwnd, width, height, (unsigned)hr);
			return false;
		}

		hr = swap_chain1.As(&swap_chain_);
		if (FAILED(hr)) {
			ELOG(L"Failed to query SwapChain4");
			return false;
		}
	}

	if (!InitSizeDependedResources(device, width, height)) {
		return false;
	}

	return true;
}

bool RenderOutput::BeginDraw(IDXDevice* device, IDXGraphicsCommandList* command_list) {
	auto [width, height] = GetClientSize(hwnd_);
	if (width != width_ || height != height_) {
		if (!Resize(device, width, height)) {
			return false;
		}
	}

	back_buffer_index_ = static_cast<int>(swap_chain_->GetCurrentBackBufferIndex());

	auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(render_targets_[back_buffer_index_].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	command_list->ResourceBarrier(1, &barrier);

	const float clear_color[4] = { 1.f, 1.f, 1.f, 1.f };
	command_list->ClearRenderTargetView(render_target_descriptor_[back_buffer_index_], clear_color, 0, nullptr);
	command_list->ClearDepthStencilView(depth_stencil_descriptor_, D3D12_CLEAR_FLAG_DEPTH, 1.f, 0, 0, nullptr);

	command_list->OMSetRenderTargets(1, &render_target_descriptor_[back_buffer_index_], FALSE, &depth_stencil_descriptor_);
	command_list->RSSetViewports(1, &viewport_);
	command_list->RSSetScissorRects(1, &scissor_);

	return true;
}

bool RenderOutput::EndDraw(IDXGraphicsCommandList* command_list) {
	auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(render_targets_[back_buffer_index_].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	command_list->ResourceBarrier(1, &barrier);

	return true;
}

void RenderOutput::Present() {
	swap_chain_->Present(1, 0);
}

bool RenderOutput::Resize(IDXDevice* device, int width, int height) {
	if (width < 1 || height < 1) {
		return true;
	}

	for (auto& render_target : render_targets_) {
		render_target.Reset();
	}

	auto hr = swap_chain_->ResizeBuffers(NumBackBuffers, width, height, RenderTargetFormat, 0);
	if (FAILED(hr)) {
		return false;
	}

	if (!InitSizeDependedResources(device, width, height)) {
		return false;
	}

	return true;
}

bool RenderOutput::InitSizeDependedResources(IDXDevice* device, int width, int height) {
	{
		width_ = width;
		height_ = height;
	}

	for (int i = 0; i < NumBackBuffers; ++i) {
		auto hr = swap_chain_->GetBuffer(i, IID_PPV_ARGS(&render_targets_[i]));
		if (FAILED(hr)) {
			return false;
		}

		device->CreateRenderTargetView(render_targets_[i].Get(), nullptr, render_target_descriptor_[i]);
	}

	{
		viewport_.Width = static_cast<float>(width);
		viewport_.Height = static_cast<float>(height);

		scissor_.right = width;
		scissor_.bottom = height;

		back_buffer_index_ = swap_chain_->GetCurrentBackBufferIndex();
	}

	{
		auto prop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
		auto desc = CD3DX12_RESOURCE_DESC::Tex2D(DepthStencilFormat, width, height, 1, 1, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);
		auto clear= CD3DX12_CLEAR_VALUE(DepthStencilFormat, 1.f, 0);
		auto hr = device->CreateCommittedResource(&prop, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &clear, IID_PPV_ARGS(&depth_stencil_));
		if (FAILED(hr)) {
			return false;
		}

		D3D12_DEPTH_STENCIL_VIEW_DESC view_desc{};
		view_desc.Format = DepthStencilFormat;
		view_desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		view_desc.Flags = D3D12_DSV_FLAG_NONE;
		device->CreateDepthStencilView(depth_stencil_.Get(), &view_desc, depth_stencil_descriptor_);
	}

	return true;
}

}