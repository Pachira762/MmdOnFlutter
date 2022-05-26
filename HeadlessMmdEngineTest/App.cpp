#include "pch.h"
#include "App.h"
#include "..\HeadlessMmdEngine\HeadlessMmdEngine.h"
#include "..\HeadlessMmdEngine\MmdScene.h"
#include "..\HeadlessMmdEngine\CameraAnimation.h"


headless_mmd::Vector operator-(const headless_mmd::Vector& a, const headless_mmd::Vector& b) {
	return DirectX::XMVectorSubtract(a, b);
}

namespace headless_mmd {

}

struct Timer {
	std::chrono::system_clock::time_point t0 = std::chrono::system_clock::now();

	void Start() {
		t0 = std::chrono::system_clock::now();
	}

	void Stop() {
		auto t1 = std::chrono::system_clock::now();
		DLOG(L"{}ms", std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count());
		t0 = t1;
	}
};

int cx = 0;
headless_mmd::ModelInfo model_info{};
headless_mmd::AnimationInfo animation_info{};
int num_morphs = 0;
std::vector<float> morph_values{};

LRESULT CALLBACK ChildWndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
	return DefWindowProc(hwnd, msg, wp, lp);
}

App::~App() {
	if (engine_) {
		engine_->Stop();
	}
}

bool App::OnCreate(HWND hwnd) {
	WNDCLASSEX wc{};
	wc.cbSize = sizeof(wc);
	wc.style = QueryWindowClassStyle();
	wc.lpfnWndProc = ChildWndProc;
	wc.hInstance = GetModuleHandle(NULL);
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszClassName = L"ChildWindowClass";
	if (RegisterClassEx(&wc) == INVALID_ATOM) {
		return false;
	}

	child_ = CreateWindowEx(0, L"ChildWindowClass", L"Child", WS_VISIBLE | WS_CHILD | WS_DISABLED,
		50, 50, 1280, 720, hwnd, NULL, GetModuleHandle(NULL), nullptr);
	if (!child_) {
		return false;
	}

	engine_ = headless_mmd::HeadlessMmdEngine::GetInstance();
	if (!engine_) {
		return false;
	}

	Timer timer{};
	if (!engine_->Start(child_)) {
		DLOG(L"Failed to initi headless mmd engine");
		return false;
	}
	DLOG(L"init engine");
	timer.Stop();

	if (!engine_->LoadModel(LR"(D:\Assets\Models\“V‰¹‚©‚È‚½ŒöŽ®mmd_ver1.0\PMX\“V‰¹‚©‚È‚½.pmx)", model_info)) {
		return false;
	}
	DLOG(L"load model");
	timer.Stop();

	num_morphs = static_cast<int>(model_info.morph_names.size());
	morph_values.resize(num_morphs);

	if (!engine_->LoadScene(LR"(C:\Users\goila\Desktop\TeBeBoy_AmaneKanata.mscap)", animation_info)) {
		return false;
	}
	DLOG(L"load scene");
	timer.Stop();

	auto scene = headless_mmd::MmdSceneImporter().Import(LR"(D:\Assets\Models\Alicia\MMD\loop1.mscap)");
	if (!scene) {
		return false;
	}

	auto cam = headless_mmd::CameraAnimationImporter().Import(*scene);
	if (!cam) {
		return false;
	}

	headless_mmd::Vector pos{}, fwd{}, up{};
	float fov{};
	for (int i = -5; i < 10; ++i) {
		headless_mmd::Vector pos1{}, fwd1{}, up1{};
		float fov1{};
		cam->GetCameraInfo(i, pos1, fwd1, up1, fov1);

		DLOG(L"#{} {} {} {} {}", i, pos1 - pos, fwd1 - fwd, up1 - up, fov1 - fov);
		pos = pos1;
		fwd = fwd1;
		up = up1;
		fov = fov1;
	}

	return true;
}

void App::OnSize(int width, int height) {
	cx = width;
	SetWindowPos(child_, NULL, 50, 50, width - 100, height - 100, SWP_NOZORDER);
}

void App::OnMouseDrag(int mx, int my, int dx, int dy) {
	auto cur = animation_info.num_frames * mx / cx;

	for (int i = 0; i < num_morphs; ++i) {
		morph_values[i] = animation_info.morph_animation[i].CalcAt(cur);
	}

	engine_->Update(cur, morph_values);
}
