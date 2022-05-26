#include "pch.h"
#include "WinAppHelper.h"
#include "SceneCaptor.h"

class MmdSceneCaptor : public MMDPluginDLL3 {
public:
	MmdSceneCaptor(IDirect3DDevice9* device) : device_(device) {
	}

private:
	virtual const char* getPluginTitle() const override {
		return "MmdSceneCaptor";
	}

	virtual void start() override;
	virtual void EndScene()override;
	virtual void MsgProc(int code, MSG* param)override;

	void StartCapture();
	void StopCapture();

	IDirect3DDevice9* device_;
	int	mid_start_capture_ = -1;
	int	mid_stop_capture_ = -1;
	bool capturing_ = false;
	std::unique_ptr<SceneCaptor> captor_ = std::make_unique<SceneCaptor>();
};

void MmdSceneCaptor::start() {
	auto eng = ExpGetEnglishMode();
	auto hsubmenu = AddSubMenu(eng ? L"SceneCaptor" : L"シーンキャプチャ");
	mid_start_capture_ = AddMenuItem(hsubmenu, eng ? L"Start Capture" : L"キャプチャ開始");
	mid_stop_capture_ = AddMenuItem(hsubmenu, eng ? L"Stop Capture" : L"キャプチャ終了");
}

void MmdSceneCaptor::EndScene() {
	if (capturing_) {
		captor_->Capture();
	}
}

void MmdSceneCaptor::MsgProc(int code, MSG* param) {
	static auto selected_id = LOWORD(0);

	if (auto msg = param->message; msg == WM_MENUSELECT) {
		selected_id = LOWORD(param->wParam);
	}
	else if (msg == WM_LBUTTONUP) {
		if (selected_id == mid_start_capture_) {
			StartCapture();
		}
		else if (selected_id == mid_stop_capture_) {
			StopCapture();
		}
	}
}

void MmdSceneCaptor::StartCapture() {
	if (!capturing_) {
		capturing_ = captor_->Start();
	}
}

void MmdSceneCaptor::StopCapture() {
	if (capturing_) {
		captor_->Stop();
		capturing_ = false;
	}
}

int version() {
	return 3; 
}

MMDPluginDLL3* create3(IDirect3DDevice9* device) {
	return new MmdSceneCaptor(device);
}

void destroy3(MMDPluginDLL3* p) {
	delete p;
}
