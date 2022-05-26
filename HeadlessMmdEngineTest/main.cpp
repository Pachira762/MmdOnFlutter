#include "pch.h"
#include "App.h"

#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "HeadlessMmdEngine.lib")

#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

struct Initializer {
	bool co_initialized;

	Initializer() {
		InitCommonControls();
		co_initialized = SUCCEEDED(CoInitialize(NULL));
	}

	~Initializer() {
		if (co_initialized) {
			CoUninitialize();
		}
	}
};

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nShowCmd) {
	Initializer initialzier{};

	try {
		return App().Run();
	}
	catch (const std::exception& e) {
		DLOG(L"CatchException!\n{}\n" ,e.what());
	}

	return -1;
}
