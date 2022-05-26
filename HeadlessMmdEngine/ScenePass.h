#pragma once
#include "DxContext.h"
#include "Scene.h"
#include "MeshPass.h"

namespace headless_mmd {

class ScenePass {
public:
	bool Init(DxContext* context, IDXRootSignature* root_signature);
	void AddPass(DxContext* context, IDXGraphicsCommandList* command_list, const Scene* scene);

private:
	MeshPass	mesh_pass_{};
};

}