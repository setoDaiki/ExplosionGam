#pragma once
#include "CameraMovement.h"

class TestCamera :public CameraMovement {
public:
	TestCamera();
	
	//-------------------------------------
	/// [�@�\]�J�����̏�Ԃ̍X�V
	//-------------------------------------
	void Excute(DirectX::XMFLOAT3& cameraPos,DirectX::XMFLOAT3& lookPos)override;
	

private:
	void Move(DirectX::XMFLOAT3& cameraPos);
	void Rotate();
	void UpdateLookPos(DirectX::XMFLOAT3& cameraPos,DirectX::XMFLOAT3& lookPos);

	DirectX::XMVECTOR mQuaternion;
	float mRoll;
};
