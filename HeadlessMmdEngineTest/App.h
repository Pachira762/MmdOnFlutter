#pragma once
#include <wrl.h>
#include <d2d1.h>
#include "WinApp.h"
#include "..\HeadlessMmdEngine\HeadlessMmdEngine.h"

class App : public winapp::WinApp {
public:
	virtual ~App();

private:
	headless_mmd::HeadlessMmdEngine* engine_ = nullptr;

	virtual bool OnCreate(HWND hwnd) override;
	virtual void OnSize(int width, int height) override;
};
