#pragma once
#include "MathHelper.h"
#include "CameraAnimation.h"

namespace headless_mmd {

class Camera {
public:
	void Update(int frame);
	void SetPosition(float x, float y, float z);
	void SetForward(float x, float y, float z);
	void SetUp(float x, float y, float z);
	void SetFoV(float fov);
	void SetAspect(float aspect);
	void SetNearFar(float clip_near, float clip_far);
	void SetAnimation(const std::shared_ptr<CameraAnimation>& animation);

private:
	void CalcViewMatrix();
	void CalcProjectionMatrix();

	Vector	position_{0.f, 10.f, -50.f};
	Vector	forward_{ 0.f, 0.f, 1.f };
	Vector	up_{ 0.f, 1.f, 0.f };
	float	fov_ = 30.f;
	float	aspect_ = 1.f;
	float	near_ = 0.1f;
	float	far_ = 1000.f;
	Matrix	view_ = matrix_identity();
	Matrix	projection_ = matrix_identity();

	std::shared_ptr<CameraAnimation> animation_{};

public:
	Vector GetPosition() const {
		return position_;
	}

	Vector GetForward() const {
		return forward_;
	}

	Vector GetUp() const {
		return up_;
	}

	float GetFoV() const {
		return fov_;
	}

	Matrix GetViewMatrix() const {
		return view_;
	}

	Matrix GetProjectionMatrix() const {
		return projection_;
	}
};

}