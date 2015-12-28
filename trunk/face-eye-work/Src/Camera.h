// Copyright (C) 2009 Jorge Jimenez (jim@unizar.es)

#ifndef CAMERA_H
#define CAMERA_H

class Camera
{
public:
	Camera(){}
	Camera(float fov, float ratio, float _near, float _far);
	~Camera();

	void update(float dt);
	void updateInput(float dt);
	void setRatio(float _ratio);
	void yaw(float angle); // rotate around z
	void pitch(float angle); // rotate around x        lock Y Euler
	GET_SET_FUNC(DirectX::MXMFLOAT3, position, Position);
	GET_SET_FUNC(DirectX::MXMFLOAT3, euler, Euler);
	GET_FUNC(DirectX::XMFLOAT4X4, view, ViewMatrix);
	GET_FUNC(DirectX::XMFLOAT4X4, projection, ProjectionMatrix);
	GET_FUNC(DirectX::XMFLOAT4X4, viewProjection, ViewProjectinoMatrix);
	GET_FUNC(DirectX::MXMFLOAT3, direction, Dir);
	GET_FUNC(DirectX::MXMFLOAT3, up, Up);
	GET_FUNC(DirectX::MXMFLOAT3, right, Right);
	GET_FUNC(float, fovY, FOVY);
	GET_FUNC(float, ratio, Ratio);
	GET_FUNC(float, nearClip, Near);
	GET_FUNC(float, farClip, Far);

	void moveForward(float dist);
	void moveBackward(float dist);
	void moveUp(float dist);
	void moveDown(float dist);
	void moveLeft(float dist);
	void moveRight(float dist);
};

#endif
