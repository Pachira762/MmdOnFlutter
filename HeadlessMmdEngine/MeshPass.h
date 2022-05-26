#pragma once
#include "DxContext.h"
#include "Mesh.h"

namespace headless_mmd {

class MeshPass {
public:
	bool Init(DxContext* context, IDXRootSignature* root_signature);
	void AddPass(DxContext* context, IDXGraphicsCommandList* command_list, const std::vector<const Mesh*>& meshes);

private:
	IDXPipelineStatePtr	state_{};
};

}