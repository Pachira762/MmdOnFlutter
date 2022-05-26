#pragma once
#include <memory>
#include <vector>
#include <set>
#include <map>
#include <string>
#include <functional>
#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>
#include <algorithm>
#include <format>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <windowsx.h>
#include <CommCtrl.h>
#include <shellapi.h>
#include <wrl.h>

#include "util.h"

#ifdef _DEBUG
inline constexpr bool IsDebug = true;
#define DLOG(...) OutputDebugStringW((std::format(__VA_ARGS__) + L"\n").c_str());
#define ELOG(...) DLOG(__VA_ARGS__)
#else
inline constexpr bool IsDebug = false;
#define DLOG(...) __noop
#define ELOG(...) MessageBox(NULL, std::format(__VA_ARGS__).c_str(), L"Error", MB_OK);
#endif

#define verify(condition) if(!condition) { throw std::runtime_error(std::format("{}({})", __FILE__, __LINE__)); }

#define verify_hr(hr) verify(SUCCEEDED(hr))
