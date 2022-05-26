#include "pch.h"
#include "CameraAnimation.h"
#include "MmdScene.h"
#include "MathHelper.h"

namespace headless_mmd {

void DecodeCameraKey(const Vector& in_position, const Vector& in_rotation, const float in_distance, const int32_t in_view_angle, Vector& position, Vector& forward, Vector& up, float& fov) {
	auto [pitch, yaw, roll] = vector_components(in_rotation);
	auto quat = DirectX::XMQuaternionRotationRollPitchYaw(-pitch, -yaw, -roll);

	forward = DirectX::XMVector3Rotate(Vector{ 0,0, 1.f }, quat);
	up = DirectX::XMVector3Rotate(Vector{ 0,1.f,0 }, quat);
	position = vector_add(in_position, vector_mul(forward, in_distance));
	fov = static_cast<float>(in_view_angle);
}

void CameraAnimation::GetCameraInfo(const int frame, Vector& position, Vector& forward, Vector& up, float& fov) const {
	auto[prev, next, s] = track_.SearchNearValue(frame);

	if (s == 0.f) {
		DecodeCameraKey(prev.position, prev.rotation, prev.distance, prev.view_angle, position, forward, up, fov);
	}
	else {
		const float vx = portable_mmd::BezierInterp(s, next.ix);
		const float vy = portable_mmd::BezierInterp(s, next.iy);
		const float vz = portable_mmd::BezierInterp(s, next.iz);
		const float vr = portable_mmd::BezierInterp(s, next.ir);
		const float vd = portable_mmd::BezierInterp(s, next.id);
		const float vv = portable_mmd::BezierInterp(s, next.iv);

		DecodeCameraKey(
			lerp(prev.position, next.position, Vector{ vx, vy, vz, 0.f }),
			lerp(prev.rotation, next.rotation, vr),
			lerp(prev.distance, next.distance, vd),
			lerp(prev.view_angle, next.view_angle, vv),
			position, forward, up, fov);
	}
}

std::shared_ptr<CameraAnimation> CameraAnimationImporter::Import(const MmdScene& scene) {
	auto animation = std::make_shared<CameraAnimation>();
	animation->track_ = scene.GetCameraTrack();

	return animation;
}

}
