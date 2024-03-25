#pragma once
#include "HitBox.h"
#include <iostream>
#include <memory>

class SphereHitBox :public HitBox {
public:
	SphereHitBox(const DirectX::XMFLOAT3& ownerPos,float radius);
	~SphereHitBox()override;

	//------------------------------------------------------------
	/// [�@�\] ���Ƃ̏Փ˔���
	/// [����] ���̃C���X�^���X�ƏՓ˂𔻕ʂ��鋅
	/// [�߂�l] �Փ˂��Ă�����true�A�Փ˂��Ă��Ȃ��Ȃ�false
	//------------------------------------------------------------
	bool IsHitWithSphere(const SphereHitBox& sphere)const override;


	//-------------------------------------------------------------
	/// [�@�\] �����蔻��̔��a�̎擾
	/// [�߂�l] �����蔻��̔��a
	//-------------------------------------------------------------
	float GetRadius()const { return mRadius; }

	

private:
	float mRadius;               // �����蔻��̔��a
};

using SpherePtrU = std::unique_ptr<SphereHitBox>;