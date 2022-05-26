#pragma once
#include "DirectXMath.h"
#include "portable_mmd.h"
#include "HeadlessMmdEngine.h"

namespace headless_mmd {

using Vector = DirectX::XMVECTOR;
using Quaternion = DirectX::XMVECTOR;
using Float2 = DirectX::XMFLOAT2;
using Float3 = DirectX::XMFLOAT3;
using Float4 = DirectX::XMFLOAT4;
using Float4x4 = DirectX::XMFLOAT4X4;
using Matrix = DirectX::XMMATRIX;

using Pmx = portable_mmd::Pmx<Vector, Vector, Vector>;
using Vmd = portable_mmd::Vmd<Vector, Vector>;

constexpr inline int Slot_Scene = 0;
constexpr inline int Slot_Model = 1;
constexpr inline int Slot_Material = 2;
constexpr inline int Slot_MaterialIndex = 3;
constexpr inline int Slot_Textures = 4;

}

template<> struct std::formatter<headless_mmd::Vector, wchar_t> : std::formatter<std::wstring, wchar_t> {
	auto format(const headless_mmd::Vector& v, wformat_context& ctx) {
		auto [x, y, z, w] = v.m128_f32;
		return formatter<std::wstring, wchar_t>::format(std::format(L"{}, {}, {}, {}", x, y, z, w), ctx);
	}
};

template<> struct std::formatter<headless_mmd::Matrix, wchar_t> : std::formatter<std::wstring, wchar_t> {
	auto format(const headless_mmd::Matrix& m, wformat_context& ctx) {
		const auto& r = m.r;
		return formatter<std::wstring, wchar_t>::format(std::format(L"{}, {}, {}, {}", r[0], r[1], r[2], r[3]), ctx);
	}
};
