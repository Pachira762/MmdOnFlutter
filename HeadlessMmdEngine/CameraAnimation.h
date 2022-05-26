#pragma once
#include <vector>
#include "Common.h"

namespace headless_mmd {

class CameraAnimation {
public:
	void GetCameraInfo(const int frame, Vector& position, Vector& forward, Vector& up, float& fov) const;

private:
	Track<Vmd::CameraKey> track_{};

	friend class CameraAnimationImporter;
};

class CameraAnimationImporter {
public:
	std::shared_ptr<CameraAnimation> Import(const class MmdScene& scene);
};

}