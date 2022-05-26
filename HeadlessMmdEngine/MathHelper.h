#pragma once
#include <tuple>
#include "DirectXMath.h"
#include "Common.h"

namespace headless_mmd {

using Vector = DirectX::XMVECTOR;
using Quaternion = DirectX::XMVECTOR;
using Matrix = DirectX::XMMATRIX;

//
// scale
//
template<typename T>
inline T lerp(const T& a, const T& b, float t) {
	return static_cast<T>((1.f - t) * static_cast<float>(a) + t * static_cast<float>(b));
}

//
// vector
//
inline std::tuple<float, float, float> vector_components(const Vector& v) {
	return { DirectX::XMVectorGetX(v),DirectX::XMVectorGetY(v) ,DirectX::XMVectorGetZ(v) };
}

inline Vector vector_zero() {
	return DirectX::XMVectorZero();
}

inline Vector vector_add(const Vector& v1, const Vector& v2) {
	return DirectX::XMVectorAdd(v1, v2);
}

inline Vector vector_mul(const Vector& v, const float s) {
	return DirectX::XMVectorScale(v, s);
}

inline Vector vector3_normalize(float x, float y, float z) {
	return DirectX::XMVector3Normalize({x, y, z});
}

inline Vector vector3_normalize(const Vector& v) {
	return DirectX::XMVector3Normalize(v);
}

inline Vector lerp(const Vector& v0, const Vector& v1, float t) {
	return DirectX::XMVectorLerp(v0, v1, t);
}

inline Vector lerp(const Vector& v0, const Vector& v1, const Vector& t) {
	return DirectX::XMVectorLerpV(v0, v1, t);
}

inline void store(Float2& dest, const Vector& src) {
	DirectX::XMStoreFloat2(&dest, src);
}

inline void store(Float3& dest, const Vector& src) {
	DirectX::XMStoreFloat3(&dest, src);
}

inline void store(Float4& dest, const Vector& src) {
	DirectX::XMStoreFloat4(&dest, src);
}

//
// quaternion
//
inline Quaternion quat_zero() {
	return DirectX::XMQuaternionIdentity();
}

//
// matrix
//
inline Matrix matrix_identity() {
	return DirectX::XMMatrixIdentity();
}

inline Matrix matrix_inverse(const Matrix& m) {
	return DirectX::XMMatrixInverse(nullptr, m);
}

inline Matrix matrix_transpose(const Matrix& m) {
	return DirectX::XMMatrixTranspose(m);
}

inline Matrix matrix_translation(float x, float y, float z) {
	return DirectX::XMMatrixTranslation(x, y, z);
}

inline Matrix matrix_translation(const Vector& v) {
	return DirectX::XMMatrixTranslation(DirectX::XMVectorGetX(v), DirectX::XMVectorGetY(v), DirectX::XMVectorGetZ(v));
}

inline void store(Float4x4& dest, const Matrix& src) {
	DirectX::XMStoreFloat4x4(&dest, src);
}

inline void store_transposed(Float4x4& dest, const Matrix& src) {
	DirectX::XMStoreFloat4x4(&dest, DirectX::XMMatrixTranspose(src));
}

}