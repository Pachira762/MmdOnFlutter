#include "pch.h"
#include "Animation.h"
#include "MmdScene.h"
#include "MathHelper.h"

namespace headless_mmd {

std::shared_ptr<Animation> AnimationImporter::Import(const struct MmdAnimationTrack& in_animation, const Skeleton& skeleton, const std::vector<std::wstring>& morph_names) {
	auto animation = std::make_shared<Animation>();
	
	ImportBoneAnimation(in_animation.bone_animation, skeleton, animation->bone_animation_);
	ImportMorphAnimation(in_animation.morph_animation, morph_names, animation->morph_animation_);

	int num_frames = 0;
	for (auto& track : animation->bone_animation_) {
		num_frames = std::max<int>(num_frames, static_cast<int>(track.keys.size()));
	}
	animation->num_frames_ = num_frames;

	return animation;
}

void AnimationImporter::ImportBoneAnimation(const std::map<std::wstring, std::vector<Matrix>>& in_animation, const Skeleton& skeleton, std::vector<Animation::BoneTrack>& animation) {
	auto bone_names = skeleton.GetBoneNames();
	const auto num_bones = static_cast<int>(bone_names.size());
	animation.resize(num_bones);

	for (int bi = 0; bi < num_bones; ++bi) {
		auto& name = bone_names.at(bi);
		auto& keys = animation.at(bi).keys;

		auto it = in_animation.find(name);
		if (it == in_animation.end() || it->second.empty()) {
			keys.emplace_back(0, skeleton.GetRefPose(bi)); // dummy key
			continue;
		}

		auto& in_track = it->second;
		const auto num_keys = static_cast<int>(in_track.size());
		keys.resize(num_keys);

		for (int ki = 0; ki < num_keys; ++ki) {
			keys[ki].frame = ki;
			keys[ki].value = in_track[ki];
		}
	}
}

void AnimationImporter::ImportMorphAnimation(const std::map<std::wstring, Track<float>>& in_animation, const std::vector<std::wstring>& morph_names, std::vector<Animation::MorphTrack>& animation) {
	const auto num_morphs = static_cast<int>(morph_names.size());
	animation.resize(num_morphs);

	for (int mi = 0; mi < num_morphs; ++mi) {
		auto& name = morph_names.at(mi);
		auto& track = animation.at(mi);

		if (auto it = in_animation.find(name); it == in_animation.end() || it->second.IsEmpty()) {
			track.keys.emplace_back(0, 0.f); // dummy key
			continue;
		}
		else {
			track = it->second;
		}
	}
}


}
