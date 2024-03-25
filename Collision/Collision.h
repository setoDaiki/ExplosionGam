#pragma once
#include <vector>
#include <string>
#include <memory>
#include <DirectXMath.h>


class Collision {
public:
	Collision(std::shared_ptr<class SphereHitBox> hitBox);
	~Collision();

	
	//----------------------------------------------------------------------------------
	/// [�@�\] �I�u�W�F�N�g���Փ˂��������׏Փ˂����S�ẴI�u�W�F�N�g�̃^�O����Ԃ�
	/// [����] �Փ˂����m����I�u�W�F�N�g(�����̃I�u�W�F�N�g�����̂Ɠ������������ׂ�)
	/// [�߂�l] �Փ˂����S�ẴI�u�W�F�N�g�̃^�O��
	/// [�I�o�[���C�h�ŕω����Ă���_] ������hitBox�̌p����
	//----------------------------------------------------------------------------------
	std::vector<class GameObject*> GetAllHitObjectsInfo();

	//----------------------------------------------------------------------------------
	/// [�@�\] �I�u�W�F�N�g���Փ˂��������׍ŏ��ɏՓ˂����I�u�W�F�N�g�̃^�O����Ԃ�
	/// [����] �Փ˂����m����I�u�W�F�N�g(�����̃I�u�W�F�N�g�����̂Ɠ������������ׂ�)
	/// [�߂�l] �ŏ��ɏՓ˂����I�u�W�F�N�g�̃^�O��
	/// [�I�[�o�[���C�h�ŕω����Ă���_] ������hitBox�̌p����
	//----------------------------------------------------------------------------------
	class GameObject* GetHitObjectInfo();
	/*��ň�����Sphere�ȊO�̏Փˌ��m�֐������ׂ�*/


private:
	void RemoveMyHitBox();
	std::shared_ptr<class HitBox> mHitBox;
	static std::vector<std::shared_ptr<class SphereHitBox>> mHitBoxs;  // �Q�[���ɂ���S�Ă̓����蔻��
};

bool IsHitWall(const DirectX::XMFLOAT3& pos);                 // �ǂɂԂ������̂���Ԃ�
bool IsHitFrontOrBackWall(const DirectX::XMFLOAT3& pos);      // �O��̕ǂɂԂ������̂���Ԃ�
bool IsHitSideWall(const DirectX::XMFLOAT3& pos);             // ���E�̕ǂɂԂ������̂���Ԃ�
