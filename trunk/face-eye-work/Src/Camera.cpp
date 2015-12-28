#include "stdafx.h"
#include "Camera.h"
using namespace DirectX;

const XMFLOAT3 FOWARD_DIR = XMFLOAT3(0, 1, 0);
const XMFLOAT3 RIGHT_DIR = XMFLOAT3(1, 0, 0);
const XMFLOAT3 UP_DIR = XMFLOAT3(0, 0, 1);

Camera::Camera(float _fov, float _ratio, float _near, float _far)
{
	fovY = _fov;
	ratio = _ratio;
	nearClip = _near;
	farClip = _far;
	XMStoreFloat4x4(&projection, XMMatrixPerspectiveFovRH(fovY, ratio, nearClip, farClip));
}

Camera::~Camera()
{

}

void Camera::setRatio(float _ratio)
{
	ratio = _ratio;
	XMStoreFloat4x4(&projection, XMMatrixPerspectiveFovRH(fovY, ratio, nearClip, farClip));
}

void Camera::update(float dt)
{
	XMMATRIX rotationZ, rotationX;
	rotationX = XMMatrixRotationX(euler.x);
	rotationZ = XMMatrixRotationZ(euler.z);
	XMMATRIX rotationViewToWorld = rotationX * rotationZ;
	
	XMStoreFloat3(&direction, XMVector3Transform(XMLoadFloat3(&FOWARD_DIR), rotationViewToWorld));
	XMStoreFloat3(&right, XMVector3Transform(XMLoadFloat3(&RIGHT_DIR), rotationViewToWorld));
	XMStoreFloat3(&up, XMVector3Transform(XMLoadFloat3(&UP_DIR), rotationViewToWorld));

	XMStoreFloat4x4(&view, XMMatrixLookToRH(XMLoadFloat3(&position), XMLoadFloat3(&direction), XMLoadFloat3(&up)));
	XMStoreFloat4x4(&viewProjection, XMLoadFloat4x4(&view) * XMLoadFloat4x4(&projection));
}

void Camera::updateInput(float dt)
{

}

void Camera::yaw(float angle)
{
	euler.z += angle;
}

void Camera::pitch(float angle)
{
	euler.x += angle;
}

void Camera::moveForward(float dist)
{
	position += direction * dist;
}

void Camera::moveBackward(float dist)
{
	position -= direction * dist;
}

void Camera::moveUp(float dist)
{
	position += up * dist;
}

void Camera::moveDown(float dist)
{
	position -= up * dist;
}

void Camera::moveLeft(float dist)
{
	position -= right * dist;
}

void Camera::moveRight(float dist)
{
	position += right * dist;
}