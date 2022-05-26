#pragma once
#include <vector>
#include <map>
#include "Common.h"

namespace headless_mmd {

struct MmdAnimationTrack {
	std::wstring name{};
	std::map<std::wstring, std::vector<Matrix>> bone_animation{};
	std::map<std::wstring, Track<float>>		morph_animation{};
};

using MmdCameraTrack = Track<Vmd::CameraKey>;

class MmdScene {
private:
	std::vector<MmdAnimationTrack> animation_tracks_{};
	MmdCameraTrack camera_track_{};

	friend class MmdSceneImporter;
	
public:
	bool HasAnimationTrack() const {
		return !animation_tracks_.empty();
	}

	std::size_t GetNumAnimations() const {
		return animation_tracks_.size();
	}

	const MmdAnimationTrack& GetAnimationTrack(int index) const {
		return animation_tracks_.at(index);
	}

	bool HasCameraTrack() const {
		return !camera_track_.IsEmpty();
	}

	const MmdCameraTrack& GetCameraTrack() const {
		return camera_track_;
	}
};

class MmdSceneImporter {
public:
	std::shared_ptr<MmdScene> Import(const std::wstring& path);
};

}