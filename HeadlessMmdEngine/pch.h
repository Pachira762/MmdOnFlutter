#ifndef PCH_H
#define PCH_H

#include <memory>
#include <vector>
#include <set>
#include <map>
#include <deque>
#include <string>
#include <functional>
#include <algorithm>
#include <ranges>
#include <chrono>
#include <thread>
#include <mutex>
#include <atomic>
#include <format>
#include <filesystem>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

#include "directx/d3d12.h"
#include "directx/d3dx12.h"
#include "DirectXMath.h"

#include "portable_mmd.h"
#include "util.h"

#endif //PCH_H
