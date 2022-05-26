#include "pch.h"
#include "Model.h"

namespace headless_mmd {

void Model::Update(DxContext* context, int frame_no, const std::vector<float>& morph_values) {
	(void)context;

	auto num_morphs = skinned_mesh_->GetNumMorphs();
	if (num_morphs == static_cast<int>(morph_values.size())) {
		for (int i = 0; i < num_morphs; ++i) {
			skinned_mesh_->SetMorph(i, morph_values[i]);
		}
		skinned_mesh_->Update();
	}

	if (!animation_) {
		return;
	}

	frame_no = std::clamp<int>(frame_no, 0, animation_->GetNumFrames());
	if (frame_no == last_frame_no_) {
		return;
	}

	for (int i = 0, num_bones = animation_->GetNumBoneTracks(); i < num_bones; ++i) {
		skeleton_->SetPose(i, animation_->GetBoneKey(i, frame_no));
	}
	skeleton_->Update();

	last_frame_no_ = frame_no;
}

void Model::SetAnimation(const std::shared_ptr<Animation>& animation) {
	animation_ = animation;
	last_frame_no_ = -1;
}

std::shared_ptr<Model> ModelImporter::Import(DxContext* context, const Pmx& pmx, const std::wstring& path) {
	auto model = std::make_shared<Model>();

	auto skinned_mesh = SkinnedMeshImporter().Import(context, pmx, path, model.get());
	if (!skinned_mesh) {
		return nullptr;
	}
	model->skinned_mesh_ = skinned_mesh;

	auto skeleton = SkeletonImporter().Import(context, pmx, model.get());
	if (!skeleton) {
		return nullptr;
	}
	model->skeleton_ = skeleton;

	return model;
}

}