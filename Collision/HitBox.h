#pragma once
#include <DirectXmath.h>
#include <string>
#include <memory>

using namespace DirectX;


class HitBox{
public:
	HitBox(const DirectX::XMFLOAT3& gameObject);
	virtual ~HitBox() {};

	//------------------------------------------------------------
	/// [�@�\] ���Ƃ̏Փ˔���
	/// [����] ���̃C���X�^���X�ƏՓ˂𔻕ʂ��鋅
	/// [�߂�l] �Փ˂��Ă�����true�A�Փ˂��Ă��Ȃ��Ȃ�false
	//------------------------------------------------------------
	virtual bool IsHitWithSphere(const class SphereHitBox& sphere)const = 0;


	//----------------------------------------------------------------------------------
	/// [�@�\] �����蔻������L���Ă���I�u�W�F�N�g���擾
	/// [�߂�l] �����蔻������L���Ă���I�u�W�F�N�g
	//-----------------------------------------------------------------------------------
	const DirectX::XMFLOAT3& GetPos()const;

	
private:
	const DirectX::XMFLOAT3& mOwnerPos;
};


using HitBoxPtrU = std::unique_ptr<HitBox>;