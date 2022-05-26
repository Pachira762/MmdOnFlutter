#include "pch.h"
#include "Skeleton.h"
#include "MathHelper.h"
#include "Model.h"

namespace headless_mmd {

void Skeleton::SetPose(int index, const Matrix& transform) {
	bones_.at(index).pose = transform;
}

void Skeleton::SetDelta(int index, const Matrix& transform) {
	bones_.at(index).delta = transform;
}

void Skeleton::Update() {
	const auto num_bones = static_cast<int>(bones_.size());

	for (int i = 0; i < num_bones; ++i) {
		const auto& bone = bones_[i];
		store_transposed(constants_->transforms[i], bone.offset * bone.delta * bone.pose);
	}
}

//
// SkeletonImporter
//
std::shared_ptr<Skeleton> SkeletonImporter::Import(DxContext* context, const Pmx& pmx, Model* parent) {
	auto skeleton = std::make_shared<Skeleton>();
	skeleton->parent_ = parent;

	const auto num_bones = static_cast<int>(pmx.bones.size());
	auto& bones = skeleton->bones_;
	bones.resize(num_bones);

	for (int i = 0; i < num_bones; ++i) {
		auto& bone = bones[i];
		const auto& pmx_bone = pmx.bones[i];

		bone.name = pmx_bone.name;
		bone.ref = matrix_translation(pmx_bone.position);
		bone.offset = matrix_inverse(bone.ref);
		bone.delta = matrix_identity();
		bone.pose = bone.ref;
	}

	skeleton->constant_buffer_ = context->CreateDynamicBuffer(sizeof(Skeleton::SkeletonConstants));
	if (!skeleton->constant_buffer_) {
		return nullptr;
	}

	auto hr = skeleton->constant_buffer_->Map(0, nullptr, (void**)&skeleton->constants_);
	if (FAILED(hr)) {
		return nullptr;
	}

	skeleton->Update();

	return skeleton;
}

}