#include "TestCamera.h"
#include "Common/MyMath.h"
#include "Input/KeyInput.h"
#include "Input/Mouse.h"
#include "Common/Printer.h"
#include "Mouse.h"
#include <xInput.h>
#include <iostream>

TestCamera::TestCamera()
	:mRoll(0.0f)
	,mQuaternion(DirectX::XMQuaternionIdentity())
{
	constexpr float radianYaw = DirectX::XMConvertToRadians(0.0f);
	mQuaternion = DirectX::XMQuaternionRotationRollPitchYaw(0.0f,radianYaw,0.0f);
}

//-------------------------------------------
//  �J�����̍X�V
//-------------------------------------------
void TestCamera::Excute(DirectX::XMFLOAT3& cameraPos,DirectX::XMFLOAT3& lookPos) {
	Move(cameraPos);
	Rotate();
	UpdateLookPos(cameraPos,lookPos);
}


//---------------------------------------
//  �J�����̒����ʒu���X�V���܂�
//---------------------------------------
void TestCamera::UpdateLookPos(DirectX::XMFLOAT3& cameraPos,DirectX::XMFLOAT3& lookPos) {
	DirectX::XMVECTOR zAxis = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 1.0f);
	const DirectX::XMVECTOR frontVector = DirectX::XMVector3Rotate(zAxis,mQuaternion);
	const DirectX::XMVECTOR posVector = DirectX::XMLoadFloat3(&cameraPos);
	DirectX::XMFLOAT3 currentLookPos(posVector.m128_f32[0] + frontVector.m128_f32[0],
					 posVector.m128_f32[1] + frontVector.m128_f32[1],
					 posVector.m128_f32[2] + frontVector.m128_f32[2]);
	lookPos = currentLookPos;
}

//-------------------------------------
//  �J�������ړ�
//-------------------------------------
void TestCamera::Move(DirectX::XMFLOAT3& cameraPos) {

	DirectX::XMFLOAT3 moveVec(0.0f,0.0f,0.0f);

	// �O���ړ�
	if (KeyInput::GetKeyState('A') == EHold) {
		moveVec = Math::GetFront(mQuaternion);
	}
	// ����ړ�
	if (KeyInput::GetKeyState('D') == EHold) {
		moveVec = Math::GetBack(mQuaternion);
	}
	// ���ړ�
	if (KeyInput::GetKeyState('S') == EHold) {
		moveVec = Math::GetLeft(mQuaternion);
	}
	// �E�ړ�
	if (KeyInput::GetKeyState('W') == EHold) {
		moveVec = Math::GetRight(mQuaternion);
	}

	moveVec.y = 0.0f;  // �㉺�ړ��͖���
	Math::GetNormalize(moveVec);
	cameraPos = cameraPos + (0.3f * moveVec);

	// ���ړ�
	if (KeyInput::GetKeyState('Z') == EHold) {
		cameraPos.y = cameraPos.y - 0.1f;
	}
	// ��ړ�
	if (KeyInput::GetKeyState('X') == EHold) {
		cameraPos.y = cameraPos.y + 0.1f;
	}

	
}

//--------------------------------------
//  �J��������]������
//--------------------------------------
void TestCamera::Rotate() {
	Mouse::Update();
	POINT point = Mouse::GetRelativeMovement();
	DirectX::XMFLOAT2 fPoint(static_cast<float>(point.x), static_cast<float>(point.y));
	fPoint.x *= 0.1f;
	fPoint.y *= 0.1f;
	mRoll += fPoint.y;
	// ���[����]�ʂ𐧌�����
	if (mRoll >= 40.0f)
	{
		fPoint.y = 0.0f;
		mRoll = 40.0f;
	}
	if (mRoll <= -10.0f) {
		fPoint.y = 0.0f;
		mRoll = -10.0f;
	}

	// ��]�ʂ��X�V
	mQuaternion = Math::QuaternionRotateEulerAndMultiply(fPoint.y, 0.0f, 0.0f, mQuaternion);
	mQuaternion = Math::QuaternionRotateEulerAndMultiply(0.0f, fPoint.x, 0.0f, mQuaternion);
}