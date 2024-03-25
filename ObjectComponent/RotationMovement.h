#pragma once
#include <DirectXMath.h>
#include <iostream>

class RotationMovement {
public:
	RotationMovement(const DirectX::XMVECTOR& rotateAxis,float rotationRatePerSecond);
	DirectX::XMVECTOR Update(const DirectX::XMVECTOR& quaternion);

private:
	DirectX::XMVECTOR mRotationAxis;
	float mRotationRatePerFlame;
};
