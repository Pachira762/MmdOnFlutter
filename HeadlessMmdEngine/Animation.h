#pragma once
#include <vector>
#include <string>
#include "Common.h"
#include "Skeleton.h"

namespace headless_mmd {

class Animation {
public:
	using BoneKey = Matrix;
	using BoneTrack = Track<BoneKey>;

	using MorphKey = float;
	using MorphTrack = Track<MorphKey>;

private:
	int num_frames_ = 0;
	std::vector<BoneTrack>	bone_animation_;
	std::vector<MorphTrack>	morph_animation_;

	friend class AnimationImporter;

public:
	int GetNumBoneTracks() const {
		return static_cast<int>(bone_animation_.size());
	}

	BoneKey GetBoneKey(int bone_index, int frame_no) {
		return bone_animation_.at(bone_index).GetByIndex(frame_no);
	}

	int GetNumMorphTracks() const {
		return static_cast<int>(morph_animation_.size());
	}

	MorphKey GetMorphKey(int morph_index, int frame_no) {
		return morph_animation_.at(morph_index).CalcAt(frame_no);
	}
	
	int GetNumFrames() const {
		return num_frames_;
	}

	const std::vector<BoneTrack>& GetBoneAnimation() const {
		return bone_animation_;
	}

	const std::vector<MorphTrack>& GetMorphAnimation() const {
		return morph_animation_;
	}
};

class AnimationImporter {
public:
	std::shared_ptr<Animation> Import(const struct MmdAnimationTrack& in_animation, const Skeleton& skeleton, const std::vector<std::wstring>& morph_names);

private:
	void ImportBoneAnimation(const std::map<std::wstring, std::vector<Matrix>>& in_animation, const Skeleton& skeleton, std::vector<Animation::BoneTrack>& animation);
	void ImportMorphAnimation(const std::map<std::wstring, Track<float>>& in_animation, const std::vector<std::wstring>& morph_names, std::vector<Animation::MorphTrack>& animation);
};

}