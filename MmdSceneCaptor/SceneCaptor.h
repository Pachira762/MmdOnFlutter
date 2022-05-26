#pragma once
#include <memory>
#include <vector>
#include <string>
#include "ModelCaptor.h"
#include "CameraCaptor.h"

class SceneCaptor {
public:
	bool Start();
	void Stop();
    void Capture();

private:
	void Save();
	void NotifyStart();
	void NotifyStartFailed();
	void NotifySave();
	void NotifySaveFailed(const std::wstring& path);

	int start_frame_no_ = 0;
	int last_frame_no_ = -1;
	std::wstring window_title_ = {};

	std::vector<std::unique_ptr<ModelCaptor>> model_captors_;
	std::unique_ptr<CameraCaptor> camera_captor_;
};
