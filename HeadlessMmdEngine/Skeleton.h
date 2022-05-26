#pragma once
#include <vector>
#include "Common.h"
#include "DxContext.h"

namespace headless_mmd {

class Model;

class Skeleton {
public:
	void SetPose(int index, const Matrix& transform);
	void SetDelta(int index, const Matrix& transform);
	void Update();

private:
	struct SkeletonConstants {
		Float4x4 transforms[1024];
	};
	static_assert(sizeof(SkeletonConstants) % 256 == 0);

	struct Bone {
		std::wstring name;
		Matrix ref;
		Matrix offset;
		Matrix delta;
		Matrix pose;
	};

	Model* parent_{};
	std::vector<Bone> bones_{};

	IDXResourcePtr		constant_buffer_{};
	SkeletonConstants*	constants_{};

	friend class SkeletonImporter;

public:
	const Model* GetParent() const {
		return parent_;
	}

	std::vector<std::wstring> GetBoneNames() const {
		const auto num_bones = static_cast<int>(bones_.size());

		std::vector<std::wstring> names(num_bones);
		for (int i = 0; i < num_bones; ++i) {
			names[i] = bones_[i].name;
		}

		return names;
	}

	const Matrix& GetRefPose(int index) const {
		return bones_.at(index).ref;
	}

	D3D12_GPU_VIRTUAL_ADDRESS GetConstantBufferLocation() const {
		return constant_buffer_->GetGPUVirtualAddress();
	}
};

class SkeletonImporter {
public:
	std::shared_ptr<Skeleton> Import(DxContext* context, const Pmx& pmx, Model* parent);
};

}