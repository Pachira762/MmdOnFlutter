#include "pch.h"
#include "HeadlessMmdEngine.h"
#include <future>
#include "EngineThread.h"

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "DirectXTex.lib")
#pragma comment(lib, "Windowscodecs.lib")

namespace {

bool engine_started = false;
std::unique_ptr<headless_mmd::EngineThread> engine_thread_ = nullptr;

}

namespace headless_mmd {
HeadlessMmdEngine::HeadlessMmdEngine() {
}

HeadlessMmdEngine::~HeadlessMmdEngine() {
	Stop();
}

bool HeadlessMmdEngine::Start(HWND hwnd) {
	if (engine_started) {
		return false;
	}

	engine_thread_ = std::make_unique<EngineThread>();
	if (!engine_thread_->Start(hwnd)) {
		return false;
	}

	engine_started = true;
	return true;
}

void HeadlessMmdEngine::Stop() {
	engine_started = false;
	engine_thread_ = nullptr;
}

void HeadlessMmdEngine::Update(int frame) {
	if (engine_started) {
		engine_thread_->Update(frame);
	}
}

void HeadlessMmdEngine::Update(int frame, int morph_index, float morph_value) {
	if (engine_started) {
		engine_thread_->Update(frame, morph_index, morph_value);
	}
}

void HeadlessMmdEngine::Update(int frame, const std::vector<float>& morph_values) {
	if (engine_started) {
		engine_thread_->Update(frame, morph_values);
	}
}

bool HeadlessMmdEngine::LoadModel(const std::wstring& path, ModelInfo& info) {
	if (engine_started) {
		return engine_thread_->LoadModel(path, info);
	}
	else {
		return false;
	}
}

bool HeadlessMmdEngine::LoadScene(const std::wstring& path, AnimationInfo& info) {
	if (engine_started) {
		return engine_thread_->LoadScene(path, info);
	}
	else {
		return false;
	}
}

bool HeadlessMmdEngine::SaveMorphAnimation(const std::wstring& path, const std::vector<std::string>& morph_names, const std::vector<Track<float>>& animation, const std::string& model_name) {
	Vmd vmd{};
	vmd.header.name = model_name;

	const auto num_morphs = static_cast<int>(morph_names.size());
	if (animation.size() != num_morphs) {
		return false;
	}

	for (int i = 0; i < num_morphs; ++i) {
		auto& name = morph_names[i];
		auto& keys = animation[i].keys;
		auto& out = vmd.morphs[name];

		const auto num_keys = static_cast<int>(keys.size());
		out.resize(keys.size());

		for (int j = 0; j < num_keys; ++j) {
			out[j].frame = keys[j].frame;
			out[j].value = keys[j].value;
		}
	}

	return portable_mmd::SaveVmd(path, vmd);
}

bool HeadlessMmdEngine::SaveMorphAnimation(const std::wstring& path, const std::vector<std::string>& morph_names, const std::vector<std::vector<int32_t>>& frame_tracks, const std::vector<std::vector<float>>& value_tracks, const std::string& model_name) {
	Vmd vmd{};
	vmd.header.name = model_name;

	const auto num_morphs = static_cast<int>(morph_names.size());
	if (frame_tracks.size() != num_morphs || value_tracks.size() != num_morphs) {
		std::wstring msg = L"frame tracks and value tracks is must be same length\r\n";
		msg += std::format(L"name len:{} frames:{} values{}", morph_names.size(), frame_tracks.size(), value_tracks.size());
		MessageBox(NULL, msg.c_str(), L"Error", MB_OK);
		return false;
	}

	for (int i = 0; i < num_morphs; ++i) {
		auto& name = morph_names[i];
		auto& frames = frame_tracks[i];
		auto& values = value_tracks[i];

		const auto num_keys = static_cast<int>(frames.size());
		if (values.size() != num_keys) {
			continue;
		}

		auto& out = vmd.morphs[name];
		out.resize(num_keys);

		for (int j = 0; j < num_keys; ++j) {
			out[j].frame = frames[j];
			out[j].value = values[j];
		}
	}

	const auto saved = portable_mmd::SaveVmd(path, vmd);
	if (!saved) {
		auto msg = std::wstring(L"Failed to save\r\n") + path;
		MessageBox(NULL, msg.c_str(), L"Error", MB_OK);
	}

	return saved;
}

} // namespace headless_mmd
