#include "pch.h"
#include "DxCore.h"
#include "DxUtil.h"

namespace headless_mmd {

DxCore::~DxCore() {
	CloseCommandContext(direct_command_context_);
	CloseCommandContext(copy_command_context_);
}

bool DxCore::Init() {
	HRESULT hr{};

#ifdef _DEBUG
	{
		IDXDebugPtr debug{};
		hr = D3D12GetDebugInterface(IID_PPV_ARGS(&debug));
		RETURN_IF_FAILED(hr, L"Failed to create Debug\n");

		debug->EnableDebugLayer();
	}
#endif

	hr = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&device_));
	RETURN_IF_FAILED(hr, L"Failed to Create Device\n");

	if (!InitCommandContext(direct_command_context_, D3D12_COMMAND_LIST_TYPE_DIRECT)) {
		return false;
	}

	if (!InitCommandContext(copy_command_context_, D3D12_COMMAND_LIST_TYPE_COPY)) {
		return false;
	}

	return true;
}

IDXGraphicsCommandList* DxCore::BeginDirectCommand(IDXPipelineState* state) {
	if (!BeginCommandList(direct_command_context_, state)) {
		return nullptr;
	}

	return direct_command_context_.command_list.Get();
}

bool DxCore::ExececuteDirectCommand(bool wait) {
	return ExecuteCommandList(direct_command_context_, wait);
}

void DxCore::WaitDirectCommand() {
	WaitCommandQueue(direct_command_context_);
}

IDXGraphicsCommandList* DxCore::BeginCopyCommand() {
	if (!BeginCommandList(copy_command_context_, nullptr)) {
		return nullptr;
	}

	return copy_command_context_.command_list.Get();
}

bool DxCore::ExecuteCopyCommand(bool wait) {
	return ExecuteCommandList(copy_command_context_, wait);
}

void DxCore::WaitCopyCommand() {
	WaitCommandQueue(copy_command_context_);
}

bool DxCore::InitCommandContext(CommandContext& context, D3D12_COMMAND_LIST_TYPE type) {
	D3D12_COMMAND_QUEUE_DESC desc{};
	desc.Type = type;
	auto hr = device_->CreateCommandQueue(&desc, IID_PPV_ARGS(&context.command_queue));
	RETURN_IF_FAILED(hr, L"Failed to create CommandQueue");

	hr = device_->CreateCommandAllocator(type, IID_PPV_ARGS(&context.command_allocator));
	RETURN_IF_FAILED(hr, L"Failed to create CommandAllocator");

	hr = device_->CreateCommandList(0, type, context.command_allocator.Get(), nullptr, IID_PPV_ARGS(&context.command_list));
	RETURN_IF_FAILED(hr, L"Failed to create CommandList");

	RETURN_IF_FAILED(context.command_list->Close(), L"Failed to close CommandList");

	hr = device_->CreateFence(context.fence_value++, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&context.fence));
	RETURN_IF_FAILED(hr, L"Failed to create Fence");

	context.fence_event = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (!context.fence_event) {
		DLOG(L"Failed to create Fence event");
		return false;
	}

	return true;
}

void DxCore::CloseCommandContext(CommandContext& context) {
	if (context.command_queue && context.fence && context.fence_event) {
		WaitCommandQueue(context);
	}

	if (context.fence_event) {
		CloseHandle(context.fence_event);
		context.fence_event = nullptr;
	}
}

bool DxCore::BeginCommandList(CommandContext& context, IDXPipelineState* state) {
	if (FAILED(context.command_allocator->Reset())) {
		return false;
	}

	if (FAILED(context.command_list->Reset(context.command_allocator.Get(), state))) {
		return false;
	}

	return true;
}

bool DxCore::ExecuteCommandList(CommandContext& context, bool wait) {
	if (FAILED(context.command_list->Close())) {
		return false;
	}

	IDXCommandList* command_lists[] = { context.command_list.Get() };
	context.command_queue->ExecuteCommandLists(_countof(command_lists), command_lists);

	if (wait) {
		WaitCommandQueue(context);
	}

	return true;
}

void DxCore::WaitCommandQueue(CommandContext& context) {
	const auto fence_to_wait = context.fence_value++;
	if (FAILED(context.command_queue->Signal(context.fence.Get(), fence_to_wait))) {
		return;
	}

	if (context.fence->GetCompletedValue() < fence_to_wait) {
		context.fence->SetEventOnCompletion(fence_to_wait, context.fence_event);
		WaitForSingleObject(context.fence_event, INFINITE);
	}
}

}