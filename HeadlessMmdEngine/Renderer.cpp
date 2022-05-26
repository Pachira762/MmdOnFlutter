#include "pch.h"
#include "Renderer.h"
#include "DebugTimer.h"

namespace headless_mmd {

bool Renderer::Init(DxContext* context, Scene* scene) {
	(void)scene;

	Timer timer;
	{
		constexpr int num_ranges = 1;
		CD3DX12_DESCRIPTOR_RANGE1 ranges[1][num_ranges]{};
		ranges[0][0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

		CD3DX12_ROOT_PARAMETER1 params[5]{};
		params[0].InitAsConstantBufferView(0);
		params[1].InitAsConstantBufferView(1);
		params[2].InitAsConstantBufferView(2);
		params[3].InitAsConstants(1, 3);
		params[4].InitAsDescriptorTable(num_ranges, ranges[0]);

		CD3DX12_STATIC_SAMPLER_DESC sampler{};
		sampler.Init(0);

		root_signature_ = context->CreateRootSignature(params, _countof(params), &sampler, 1, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
		if (!root_signature_) {
			return false;
		}
	}
	timer.Stop(L"create root signature");

	if (!scene_pass_.Init(context, root_signature_.Get())) {
		return false;
	}
	timer.Stop(L"create pass");

	return true;
}

void Renderer::Draw(DxContext* context, Scene* scene) {
	auto command_list = context->BeginDraw();
	if (!command_list) {
		return;
	}

	DrawInternal(context, scene, command_list);

	context->EndDraw(command_list);
}

void Renderer::DrawInternal(DxContext* context, Scene* scene, IDXGraphicsCommandList* command_list) {
	command_list->SetGraphicsRootSignature(root_signature_.Get());

	IDXDescriptorHeap* descriptor_heaps[] = { context->GetDescriptorHeap() };
	command_list->SetDescriptorHeaps(_countof(descriptor_heaps), descriptor_heaps);

	scene_pass_.AddPass(context, command_list, scene);
}

}