#pragma once
#include "DxContext.h"
#include "Camera.h"
#include "Model.h"

namespace headless_mmd {

class Scene {
public:
	bool Init(DxContext* context);
	void Update(DxContext* context, int frame, const std::vector<float>& morph_values);

private:
	struct SceneConstants {
		Float4x4 view;				// v[0 - 3]
		Float4x4 projectoin;		// v[4 - 7]
		Float4x4 world_to_screen;	// v[8 - 11]

		Float4 eye_position;		// v[12]
		Float4 eye_forward;			// v[13]

		Float4 light_direction;		// v[14]
		Float4 shadow_color;		// v[15];
	};
	static_assert(sizeof(SceneConstants) == 256);

	IDXResourcePtr	constant_buffer_{};
	SceneConstants* constants_{};

	Camera camera_{};
	std::vector<std::shared_ptr<Model>> models_{};

public:
	void AddModel(const std::shared_ptr<Model>& model) {
		models_.push_back(model);
	}

	void SetCameraAnimation(const std::shared_ptr<CameraAnimation>& animation) {
		camera_.SetAnimation(animation);
	}

	std::vector<const Mesh*> GatherMeshes() const {
		std::vector<const Mesh*> meshes{};

		for (auto& model : models_) {
			for (auto& mesh : model->GetMesh()->GetMeshes()) {
				meshes.push_back(&mesh);
			}
		}

		return meshes;
	}

	D3D12_GPU_VIRTUAL_ADDRESS GetConstantBufferLocation() const {
		return constant_buffer_->GetGPUVirtualAddress();
	}
};

}