#include "pch.h"
#include "DxContextBase.h"

namespace headless_mmd {

bool DxContextBase::Init(HWND hwnd) {
	if (!core_->Init()) {
		return false;
	}

	if (!srv_heap_->Init(core_->GetDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 4096, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE)) {
		return false;
	}

	if (!rtv_heap_->Init(core_->GetDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, output_->NumBackBuffers, D3D12_DESCRIPTOR_HEAP_FLAG_NONE)) {
		return false;
	}

	if (!dsv_heap_->Init(core_->GetDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, D3D12_DESCRIPTOR_HEAP_FLAG_NONE)) {
		return false;
	}

	if (!output_->Init(core_->GetDevice(), core_->GetDirectommandQueue(), hwnd, rtv_heap_->GetCpuHandle(), dsv_heap_->GetCpuHandle())) {
		return false;
	}

	return true;
}

IDXGraphicsCommandList* DxContextBase::BeginDraw() {
	auto command_list = core_->BeginDirectCommand();
	if (!command_list) {
		DLOG(L"Failed to begin direct command list");
	}

	output_->BeginDraw(core_->GetDevice(), command_list);

	return command_list;
}

void DxContextBase::EndDraw(IDXGraphicsCommandList* command_list) {
	output_->EndDraw(command_list);

	if (!core_->ExececuteDirectCommand()) {
		DLOG(L"Failed to ExcecuteDirectCommandList");
	}

	output_->Present();
	core_->WaitDirectCommand();
}

bool DxContextBase::Execute(std::function<bool(IDXGraphicsCommandList* command_list)>&& command_func, bool wait) {
	auto command_list = core_->BeginDirectCommand();
	if (!command_list) {
		return false;
	}

	if (!command_func(command_list)) {
		return false;
	}

	if (!core_->ExececuteDirectCommand()) {
		return false;
	}

	if (wait) {
		core_->WaitDirectCommand();
	}

	return true;
}

void DxContextBase::Flush() {
	core_->WaitDirectCommand();
	core_->WaitCopyCommand();
}

}

/*

---- frame ----
 wait
				| notify B
 renderTarget A
 draw
 execute
---- frame ----
 wait 
				| notify A
 RenderTarget B
 draw
 execute
  

---- frame ----
wait previous command
draw		| notify new frame
copy
---- frame ----
wait previous commands
draw		| notify new frame
copy


---- frame ----
 wait previous draw and copy
								| notify new frame
 barrier A to render target		| copy B to staging
 draw							
 barrier A to copy source
 execute draw and copy

---- frame ----
 wait previous draw and copy
								| notify new frame
 barrier B to render target		| copy A to staging


-------------------------------


---- frame ----
 wait previous draw and copy
								| notify new frame A
 barrier A to render target		| copy B to staging B
 draw
 barrier A to copy source
 execute draw and copy

---- frame ----
 wait previous draw and copy
								| notify new frame B
 barrier B to render target		| copy A to staging A

*/