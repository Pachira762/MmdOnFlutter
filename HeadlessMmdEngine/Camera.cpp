#include "pch.h"
#include "Camera.h"

namespace headless_mmd {

void Camera::Update(int frame) {
	if (animation_) {
		animation_->GetCameraInfo(frame, position_, forward_, up_, fov_);
	}

	CalcViewMatrix();
	CalcProjectionMatrix();
}

void Camera::SetPosition(float x, float y, float z) {
	position_ = { x, y, z };
	CalcViewMatrix();
}

void Camera::SetForward(float x, float y, float z) {
	forward_ = { x, y, z };
	CalcViewMatrix();
}

void Camera::SetUp(float x, float y, float z) {
	up_ = { x, y, z };
	CalcViewMatrix();
}

void Camera::SetFoV(float fov) {
	fov_ = fov;
	CalcProjectionMatrix();
}

void Camera::SetAspect(float aspect) {
	aspect_ = aspect;
	CalcProjectionMatrix();
}

void Camera::SetNearFar(float clip_near, float clip_far) {
	near_ = clip_near;
	far_ = clip_far;
}

void Camera::SetAnimation(const std::shared_ptr<CameraAnimation>& animation) {
	animation_ = animation;
}

void Camera::CalcViewMatrix() {
	view_ = DirectX::XMMatrixLookToLH(position_, forward_, up_);
}

void Camera::CalcProjectionMatrix() {
	projection_ = DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(fov_), aspect_, near_, far_);
}

}