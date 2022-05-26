#pragma once
#include "DxContext.h"
#include "Scene.h"
#include "ScenePass.h"

namespace headless_mmd {

class Renderer {
public:
	bool Init(DxContext* context, Scene* scene);
	void Draw(DxContext* context, Scene* scene);

private:
	void DrawInternal(DxContext* context, Scene* scene, IDXGraphicsCommandList* command_list);

	IDXRootSignaturePtr	root_signature_{};

	ScenePass scene_pass_;
};

}