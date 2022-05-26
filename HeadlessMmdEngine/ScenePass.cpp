#include "pch.h"
#include "ScenePass.h"

namespace headless_mmd {

bool ScenePass::Init(DxContext* context, IDXRootSignature* root_signature) {
	if (!mesh_pass_.Init(context, root_signature)) {
		return false;
	}

	return true;
}

void ScenePass::AddPass(DxContext* context, IDXGraphicsCommandList* command_list, const Scene* scene) {
	command_list->SetGraphicsRootConstantBufferView(Slot_Scene, scene->GetConstantBufferLocation());

	const auto& meshes = scene->GatherMeshes();
	mesh_pass_.AddPass(context, command_list, meshes);
}

}
