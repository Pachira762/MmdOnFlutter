#include "pch.h"
#include "MeshPass.h"
#include "Model.h"
#include "SkinnedMesh.h"
#include "Mesh.h"
#include "Texture.h"
#include "DebugTimer.h"

namespace headless_mmd {

constexpr uint8_t vs_bin[] = {
#include "MeshPass.hlsl_MainVS.txt"
};

constexpr uint8_t ps_bin[] = {
#include "MeshPass.hlsl_MainPS.txt"
};

bool MeshPass::Init(DxContext* context, IDXRootSignature* root_signature) {
	D3D12_INPUT_ELEMENT_DESC input_layout[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"BONE_INDICES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, 32, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"BONE_WEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 48, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"VERTEX_OFFSET", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
	};

	D3D12_GRAPHICS_PIPELINE_STATE_DESC desc{};
	desc.pRootSignature = root_signature;
	desc.VS.pShaderBytecode = vs_bin;
	desc.VS.BytecodeLength = _countof(vs_bin);
	desc.PS.pShaderBytecode = ps_bin;
	desc.PS.BytecodeLength = _countof(ps_bin);
	desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	desc.BlendState.RenderTarget[0].BlendEnable = TRUE;
	desc.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	desc.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	desc.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	desc.BlendState.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	desc.BlendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ONE;
	desc.BlendState.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	desc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	desc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	desc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	desc.InputLayout.pInputElementDescs = input_layout;
	desc.InputLayout.NumElements = _countof(input_layout);
	desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	desc.NumRenderTargets = 1;
	desc.RTVFormats[0] = context->GetRenderTargetFormat();
	desc.DSVFormat = context->GetDepthStencilFormat();
	desc.SampleDesc.Count = 1;
	desc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	state_ = context->CreatePipelineState(desc);
	if (!state_) {
		DLOG(L"Failed to create mesh pass state");
		return false;
	}

	return true;
}

void MeshPass::AddPass(DxContext* context, IDXGraphicsCommandList* command_list, const std::vector<const Mesh*>& meshes) {
	(void)context;

	command_list->SetPipelineState(state_.Get());

	const Model* model = nullptr;
	const SkinnedMesh* skinned_mesh = nullptr;
	for (auto mesh : meshes) {
		if (auto parent = mesh->GetParent(); parent && parent != skinned_mesh) { // new model
			skinned_mesh = parent;
			model = skinned_mesh->GetParent();

			command_list->IASetVertexBuffers(0, skinned_mesh->GetNumVertexBuffers(), skinned_mesh->GetVertexBufferViews());
			command_list->IASetIndexBuffer(skinned_mesh->GetIndexBufferView());
			command_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			command_list->SetGraphicsRootConstantBufferView(Slot_Model, model->GetModelConstantBufferLocation());
			command_list->SetGraphicsRootConstantBufferView(Slot_Material, model->GetMaterialConstantBufferLocation());
		}

		command_list->SetGraphicsRoot32BitConstant(Slot_MaterialIndex, mesh->GetMaterialIndex(), 0);
		command_list->SetGraphicsRootDescriptorTable(Slot_Textures, mesh->GetTexture()->GetDescriptor());

		command_list->DrawIndexedInstanced(mesh->GetNumIndices(), 1, mesh->GetStartIndex(), 0, 0);
	}
}

}