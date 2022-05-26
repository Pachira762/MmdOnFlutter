#pragma once
#include <atomic>
#include "DxClasses.h"

namespace headless_mmd {

class RenderOutput {
public:
	static constexpr auto RenderTargetFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	static constexpr auto DepthStencilFormat = DXGI_FORMAT_D32_FLOAT;
	static constexpr int NumBackBuffers = 2;

	bool Init(IDXDevice* device, IDXCommandQueue* command_queue, HWND hwnd, D3D12_CPU_DESCRIPTOR_HANDLE rtv_handle, D3D12_CPU_DESCRIPTOR_HANDLE dsv_handle);

	bool BeginDraw(IDXDevice* device, IDXGraphicsCommandList* command_list);
	bool EndDraw(IDXGraphicsCommandList* command_list);
	void Present();

private:
	bool Resize(IDXDevice* device, int width, int height);
	bool InitSizeDependedResources(IDXDevice* device, int width, int height);

	static constexpr float ClearColor[4]{ 0.96f,0.97f,0.98f,0 };

	HWND hwnd_{};
	int width_ = 0;
	int height_ = 0;

	IDXFactoryPtr factory_{};
	IDXSwapChainPtr swap_chain_{};

	IDXResourcePtr	render_targets_[NumBackBuffers]{};
	D3D12_CPU_DESCRIPTOR_HANDLE render_target_descriptor_[NumBackBuffers]{};
	int back_buffer_index_ = 0;

	IDXResourcePtr depth_stencil_{};
	D3D12_CPU_DESCRIPTOR_HANDLE depth_stencil_descriptor_{};

	D3D12_VIEWPORT viewport_{ 0.f, 0.f, 0.f, 0.f, 0.f, 1.f };
	D3D12_RECT scissor_{ 0, 0, 0, 0 };

public:
	std::tuple<int, int> GetRenderTargetSize() const {
		return { width_, height_ };
	}
};

}