#pragma once
#include <memory>
#include "Common.h"
#include "DxContext.h"
#include "SkinnedMesh.h"
#include "Skeleton.h"
#include "Animation.h"
#include "Material.h"

namespace headless_mmd {

class Model {
public:
	void Update(DxContext* context, int frame_no, const std::vector<float>& morph_values);
	void SetAnimation(const std::shared_ptr<Animation>& animation);

private:
	std::shared_ptr<SkinnedMesh>	skinned_mesh_{};
	std::shared_ptr < Skeleton>		skeleton_{};
	std::shared_ptr<Animation>		animation_{};
	int last_frame_no_ = -1;

	friend class ModelImporter;

public:
	const std::shared_ptr<SkinnedMesh>& GetMesh() const {
		return skinned_mesh_;
	}

	const std::shared_ptr<Skeleton>& GetSkeleton() const {
		return skeleton_;
	}

	D3D12_GPU_VIRTUAL_ADDRESS GetModelConstantBufferLocation() const {
		return skeleton_->GetConstantBufferLocation();
	}

	D3D12_GPU_VIRTUAL_ADDRESS GetMaterialConstantBufferLocation() const {
		return skinned_mesh_->GetConstantBufferLocation();
	}
};

class ModelImporter {
public:
	std::shared_ptr<Model> Import(DxContext* context, const Pmx& pmx, const std::wstring& path);
};

}
