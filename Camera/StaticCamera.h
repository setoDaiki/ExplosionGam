#pragma once
#include "CameraMovement.h"

class StaticCamera :public CameraMovement {
public:
	StaticCamera();
	
	//-------------------------------------
	/// [�@�\]�J�����̏�Ԃ̍X�V
	//-------------------------------------
	void Excute(DirectX::XMFLOAT3& pos,DirectX::XMFLOAT3& lookPos)override;
	
private:

};
