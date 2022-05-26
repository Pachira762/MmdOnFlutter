#include "pch.h"
#include "FileWriter.h"
#include "SceneCaptor.h"
#include "WinAppHelper.h"

enum class SceneCaptureFlag : uint32_t {
	Motion = 0b0001,
	Morph =  0b0010,
	Camera = 0b0100,
};

int GetCurrentFrame() {
	return static_cast<int>(round(30.f * ExpGetFrameTime()));
}

bool SceneCaptor::Start() {
	auto num_models = ExpGetPmdNum();
	if (num_models == 0) {
		NotifyStartFailed();
		return false;
	}

	window_title_ = GetWindowTitle(getHWND());
	start_frame_no_ = GetCurrentFrame();
	last_frame_no_ = -1;

	model_captors_.clear();
	for (int i = 0; i < num_models; ++i) {
		model_captors_.emplace_back(std::make_unique<ModelCaptor>(i));
	}

	camera_captor_ = std::make_unique<CameraCaptor>();

	NotifyStart();
	return true;
}

void SceneCaptor::Stop() {
	SetWindowTitle(getHWND(), window_title_);
	Save();
}

void SceneCaptor::Capture() {
	auto data = mmp::getMMDMainData();
	if (!data) {
		return;
	}

	auto frame_no = GetCurrentFrame() - start_frame_no_;
	if (frame_no < 0 || frame_no < last_frame_no_) {
		return;
	}

	for (auto& model_captor : model_captors_) {
		model_captor->Capture(frame_no);
	}

	last_frame_no_ = frame_no;
	SetWindowTitle(getHWND(), ExpGetEnglishMode() ? (L"Capture #" + std::to_wstring(frame_no)) : (L"キャプチャ #" + std::to_wstring(frame_no)));
}

void SceneCaptor::Save(){
	auto path = OpenSaveFileDialog(ExpGetEnglishMode() ? L"Save Scene Capture Data" : L"キャプチャデータを保存", L"Mmd Scene Capture Data file(*.mscap)\0*.mscap\0", L"mscap");
	if (path.empty()) { // canceled
		return;
	}

	auto writer = FileWriter(path.c_str());
	if (!writer) {
		NotifySaveFailed(path);
		return;
	}

	// header
	{
		auto flag = static_cast<uint32_t>(SceneCaptureFlag::Motion) | static_cast<uint32_t>(SceneCaptureFlag::Morph) | static_cast<uint32_t>(SceneCaptureFlag::Camera);
		writer.Write(flag);
	}

	// model data
	{
		auto num_models = static_cast<int32_t>(model_captors_.size());
		writer.Write(num_models);

		for (auto& model_captor : model_captors_) {
			model_captor->Save(writer);
		}
	}

	// camera data
	{
		camera_captor_->Save(writer);
	}

	NotifySave();
}

void SceneCaptor::NotifyStart() {
}

void SceneCaptor::NotifyStartFailed() {
	MessageBeep(MB_ICONASTERISK);

	auto eng = ExpGetEnglishMode();
	MessageBox(NULL, eng ? L"Model is not loaded." : L"モデルが読み込まれていません", L"Mmd Scene Captor", MB_OK);
}

void SceneCaptor::NotifySave() {
	MessageBeep(MB_ICONASTERISK);
}

void SceneCaptor::NotifySaveFailed(const std::wstring& path) {
	auto eng = ExpGetEnglishMode();
	std::wstring msg = eng ? L"Failed to open file" : L"ファイルを開けませんでした";
	msg += L"\r\n" + path;
	MessageBox(NULL, msg.c_str(), L"Mmd Scene Capture", MB_OK);
}
