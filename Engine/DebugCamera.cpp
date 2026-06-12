#include "DebugCamera.h"

#include "Transform.h"
#include "MathFunctions.h"

#include <cassert>

void DebugCamera::Initialize(InputKey *keyboard, InputMouse *mouse) {
	keyboard_ = keyboard;
	mouse_ = mouse;
}

void DebugCamera::Update() {
	assert(mouse_);
	assert(keyboard_);

	CameraMove();
	CameraZoom();
	CameraRotate();

	Vector3 right = {
		sinf(rotation_.y - float(M_PI) / 2.0f),
		0.0f,
		cosf(rotation_.y - float(M_PI) / 2.0f)
	};

	Vector3 up = { 0, 1, 0 };

	Vector3 offsetTarget{
		target_.x + right.x * screenOffset_.x + up.x * screenOffset_.y,
		target_.y + right.y * screenOffset_.x + up.y * screenOffset_.y,
		target_.z + right.z * screenOffset_.x + up.z * screenOffset_.y,
	};
	
	Vector3 forwardXZ = {
		sinf(rotation_.y),
		0.0f,
		cosf(rotation_.y)
	};

	Vector3 forward = {
		forwardXZ.x * cosf(rotation_.x),
		-sinf(rotation_.x),
		forwardXZ.z * cosf(rotation_.x)
	};

	translation_ = {
		offsetTarget.x + forward.x * distance_,
		offsetTarget.y + forward.y * distance_,
		offsetTarget.z + forward.z * distance_,
	};

	Transform transform{};
	transform.scale = { 1,1,1 };
	transform.rotate = rotation_;
	transform.translate = translation_;

	Matrix4x4 worldMatrix = MakeWorldMatrix(transform);

	viewMatrix_ = Inverse(worldMatrix);

	projectionMatrix_ = MakePerspectiveFovMatrix(
		fovY_, width_ / height_, nearClip_, farClip_);

	viewProjectionMatrix_ =
		Multiply(viewMatrix_, projectionMatrix_);
}

void DebugCamera::CameraMove() {
	if (keyboard_->PushKey(DIK_W)) screenOffset_.y += moveSpeed_;
	if (keyboard_->PushKey(DIK_S)) screenOffset_.y -= moveSpeed_;
	if (keyboard_->PushKey(DIK_D)) screenOffset_.x += moveSpeed_;
	if (keyboard_->PushKey(DIK_A)) screenOffset_.x -= moveSpeed_;
}

void DebugCamera::CameraZoom() {
	int wheel = mouse_->GetWheelDelta();
	if (wheel != 0) {
		distance_ -= static_cast<float>(wheel) * 0.0005f;

		if (distance_ > 0.0f) {
			distance_ = 0.0f;
		}
	}
}

void DebugCamera::CameraRotate() {
	if (!mouse_->PushLeft()) return;

	rotation_.y += mouse_->GetDeltaX() * rotateSpeed_;
	rotation_.x += mouse_->GetDeltaY() * rotateSpeed_;

	const float limit = float(M_PI_2) - 0.01f;
	rotation_.x = std::clamp(rotation_.x, -limit, limit);
}