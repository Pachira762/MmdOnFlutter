#include "pch.h"
#include "App.h"
#include "..\HeadlessMmdEngine\HeadlessMmdEngine.h"
#include "..\HeadlessMmdEngine\MmdScene.h"
#include "..\HeadlessMmdEngine\CameraAnimation.h"

App::~App() {
	if (engine_) {
		engine_->Stop();
	}
}

bool App::OnCreate(HWND hwnd) {
	WNDCLASSEX wc{};
	wc.cbSize = sizeof(wc);
	wc.style = QueryWindowClassStyle();
	wc.lpfnWndProc = DefWindowProc;
	wc.hInstance = GetModuleHandle(NULL);
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszClassName = L"ChildWindowClass";
	if (RegisterClassEx(&wc) == INVALID_ATOM) {
		return false;
	}

	auto hview = CreateWindowEx(0, L"ChildWindowClass", L"Child", WS_VISIBLE | WS_CHILD | WS_DISABLED,
		50, 50, 1280, 720, hwnd, NULL, GetModuleHandle(NULL), nullptr);
	if (!hview) {
		return false;
	}

	engine_ = headless_mmd::HeadlessMmdEngine::GetInstance();
	if (!engine_) {
		return false;
	}

	if(!engine_->Start(hview)){ 
		return false;
	}

	//engine_->LoadModel(L"model.pmx", model_info);
	//engine_->LoadScene(L"scene.mscap", animation_info);

	return true;
}

void App::OnSize(int width, int height) {
}
