#include "RotationMovement.h"
#include "Common/MyMath.h"

RotationMovement::RotationMovement(const DirectX::XMVECTOR& rotationAxis,float rotationRatePerSecond)
	:mRotationAxis(rotationAxis)
	,mRotationRatePerFlame(rotationRatePerSecond/60)
{

}

DirectX::XMVECTOR RotationMovement::Update(const DirectX::XMVECTOR& quaternion) {
	return Math::QuaternionRotateAxisAndMultiply(mRotationAxis,mRotationRatePerFlame,quaternion); // ‰ñ“]‘¬“x‚ð‚à‚Æ‚É‰ñ“]
}