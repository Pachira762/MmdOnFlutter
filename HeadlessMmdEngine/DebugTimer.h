#pragma once
#include <chrono>
#include "util.h"

namespace headless_mmd {

struct Timer {
#ifdef _DEBUG
	std::chrono::system_clock::time_point t0 = std::chrono::system_clock::now();

	void Start() {
		t0 = std::chrono::system_clock::now();
	}

	void Stop(const std::wstring& tag = L"") {
		auto t1 = std::chrono::system_clock::now();
		DLOG(L"{} {}ms", tag, std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count());
		t0 = t1;
	}
#else
	void Start() { __noop; }
	void Stop(const std::wstring& tag = L"") { (void)tag; __noop; }
#endif
};

}
