#include "camera.h"

Camera::Camera() : m_pos(0.0f, 0.0f, 0.0f), m_rot(0.0f, 0.0f, 0.0f) {
	DirectX::XMStoreFloat4x4(&m_projectionMatrix, DirectX::XMMatrixIdentity());
	UpdateMatrix();
}

void Camera::SetProjectionValues(float fov, float aspect_ratio, float near_z, float far_z) {
	DirectX::XMStoreFloat4x4(&m_projectionMatrix, DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(fov), aspect_ratio, near_z, far_z));
}

const DirectX::XMMATRIX Camera::GetViewMatrix() const {
	return DirectX::XMLoadFloat4x4(&m_viewMatrix);
}

const DirectX::XMMATRIX Camera::GetViewMatrixT() const {
	return DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&m_viewMatrix));
}

const DirectX::XMFLOAT4X4 Camera::GetViewMatrix4x4() const {
	return m_viewMatrix;
}

const DirectX::XMFLOAT4X4 Camera::GetViewMatrix4x4T() const {
	DirectX::XMFLOAT4X4 res;
	DirectX::XMStoreFloat4x4(&res, DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&m_viewMatrix)));
	return res;
}

const DirectX::XMMATRIX Camera::GetProjectionMatrix() const {
	return DirectX::XMLoadFloat4x4(&m_projectionMatrix);
}

const DirectX::XMMATRIX Camera::GetProjectionMatrixT() const {
	return DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&m_projectionMatrix));
}

const DirectX::XMFLOAT4X4 Camera::GetProjectionMatrix4x4() const {
	return m_projectionMatrix;
}

const DirectX::XMFLOAT4X4 Camera::GetProjectionMatrix4x4T() const {
	DirectX::XMFLOAT4X4 res;
	DirectX::XMStoreFloat4x4(&res, DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&m_viewMatrix)));
	return res;
}

const DirectX::XMVECTOR Camera::GetPosition() const {
	return DirectX::XMLoadFloat3(&m_pos);
}

const DirectX::XMFLOAT3 Camera::GetPosition3() const {
	return m_pos;
}

const DirectX::XMVECTOR Camera::GetRotation() const {
	return DirectX::XMLoadFloat3(&m_rot);
}

const DirectX::XMFLOAT3 Camera::GetRotation3() const {
	return m_rot;
}

void Camera::SetPosition(DirectX::FXMVECTOR pos) {
	DirectX::XMStoreFloat3(&m_pos, pos);
	UpdateMatrix();
}

void Camera::SetPosition(float x, float y, float z) {
	m_pos.x = x;
	m_pos.y = y;
	m_pos.z = z;
	UpdateMatrix();
}

void Camera::AdjustPosition(DirectX::FXMVECTOR pos) {
	DirectX::XMStoreFloat3(&m_pos, DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&m_pos), pos));
	UpdateMatrix();
}

void Camera::AdjustPosition(float x, float y, float z) {
	m_pos.x += x;
	m_pos.y += y;
	m_pos.z += z;
	UpdateMatrix();
}

void Camera::SetRotation(DirectX::FXMVECTOR rot) {
	DirectX::XMStoreFloat3(&m_rot, rot);
	UpdateMatrix();
}

void Camera::SetRotation(float p, float y, float r) {
	m_rot.x = p;
	m_rot.y = y;
	m_rot.z = r;
	UpdateMatrix();
}

void Camera::AdjustRotation(DirectX::FXMVECTOR rot) {
	DirectX::XMStoreFloat3(&m_rot, DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&m_rot), rot));
	UpdateMatrix();
}

void Camera::AdjustRotation(float p, float y, float r) {
	m_rot.x += p;
	m_rot.y += y;
	m_rot.z += r;
	UpdateMatrix();
}

void Camera::SetLookAtPos(DirectX::FXMVECTOR lookAtPos) {
	DirectX::XMVECTOR pos = DirectX::XMLoadFloat3(&m_pos);
	if (DirectX::XMVector3NearEqual(lookAtPos, pos, DirectX::XMLoadFloat(&EPSILON))) { return; }

	DirectX::XMVECTOR lookAtDirection = DirectX::XMVectorSubtract(pos, lookAtPos);
	const float x = DirectX::XMVectorGetX(lookAtDirection);
	const float y = DirectX::XMVectorGetY(lookAtDirection);
	const float z = DirectX::XMVectorGetZ(lookAtDirection);


	float pitch = 0.0f;
	if (!DirectX::XMScalarNearEqual(y, 0.0f, EPSILON)) {
		const float distance = DirectX::XMVectorGetX(DirectX::XMVectorSqrt(DirectX::XMVectorSet(x * x + z * z, 0.0f, 0.0f, 0.0f)));
		pitch = DirectX::XMVectorGetX(DirectX::XMVectorATan2(DirectX::XMVectorSet(y, 0.0f, 0.0f, 0.0f), DirectX::XMVectorSet(distance, 0.0f, 0.0f, 0.0f)));
	}

	float yaw = 0.0f;
	if (!DirectX::XMScalarNearEqual(x, 0.0f, EPSILON)) {
		//yaw = DirectX::XMVectorGetX(DirectX::XMVectorATan2(DirectX::XMVectorSet(x, 0.0f, 0.0f, 0.0f), DirectX::XMVectorSet(z, 0.0f, 0.0f, 0.0f)));
		yaw = DirectX::XMVectorGetX(DirectX::XMVectorATan(DirectX::XMVectorSet(x / z, 0.0f, 0.0f, 0.0f)));
	}
	if (z > 0.0f) {
		yaw += DirectX::XM_PI;
	}
	
	SetRotation(pitch, yaw, 0.0f);
}

void Camera::SetLookAtPos(DirectX::XMFLOAT3 lookAtPos) {
	SetLookAtPos(DirectX::XMLoadFloat3(&lookAtPos));
}

void Camera::SetLookAtPos(float x, float y, float z) {
	SetLookAtPos(DirectX::XMVectorSet(x, y, z, 1.0f));
}

DirectX::XMVECTOR Camera::GetForward() {
	return DirectX::XMLoadFloat3(&m_forward);
}

DirectX::XMFLOAT3 Camera::GetForward3() {
	return m_forward;
}

DirectX::XMVECTOR Camera::GetRight() {
	return DirectX::XMLoadFloat3(&m_right);
}

DirectX::XMFLOAT3 Camera::GetRight3() {
	return m_right;
}

DirectX::XMVECTOR Camera::GetBackward() {
	return DirectX::XMLoadFloat3(&m_backward);
}

DirectX::XMFLOAT3 Camera::GetBackward3() {
	return m_backward;
}

DirectX::XMVECTOR Camera::GetLeft() {
	return DirectX::XMLoadFloat3(&m_left);
}

DirectX::XMFLOAT3 Camera::GetLeft3() {
	return m_left;
}

void Camera::UpdateMatrix() {
	DirectX::XMMATRIX camRotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(m_rot.x, m_rot.y, m_rot.z);
	DirectX::XMVECTOR camDirection = DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&DEFAULT_FORWARD_VECTOR), camRotationMatrix);
	DirectX::XMVECTOR upDir = DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&DEFAULT_UP_VECTOR), camRotationMatrix);
	DirectX::XMStoreFloat4x4(&m_viewMatrix, DirectX::XMMatrixLookToLH(DirectX::XMLoadFloat3(&m_pos), camDirection, upDir));

	DirectX::XMMATRIX rotationMartix = DirectX::XMMatrixRotationRollPitchYaw(0.0f, m_rot.y, 0.0f);
	DirectX::XMStoreFloat3(&m_forward, DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&DEFAULT_FORWARD_VECTOR), rotationMartix));
	DirectX::XMStoreFloat3(&m_backward, DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&DEFAULT_BACKWARD_VECTOR), rotationMartix));
	DirectX::XMStoreFloat3(&m_left, DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&DEFAULT_LEFT_VECTOR), rotationMartix));
	DirectX::XMStoreFloat3(&m_right, DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&DEFAULT_RIGHT_VECTOR), rotationMartix));
}