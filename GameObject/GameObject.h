#pragma once
#include <vector>
#include <memory>
#include <string>
#include <DirectXMath.h>


// �O���錾
namespace Object {
	enum Type;
};


//=====================================================
/// [�@�\] �I�u�W�F�N�g�̃N���X�ł��B
/// 3D�I�u�W�F�N�g������Ƃ��͂��̃N���X���g�p���܂�
//=====================================================
class GameObject
{
public:
	enum State {
		EActive,
		EDead,
		EStop,
		EUnDraw,
		EStopUndraw,
		EOther
	};

	
	//---------------------------------------------------------------------------------------------------
	/// [�@�\] �R���X�g���N�^�B������p����object�̏����������܂��B�ʒu�A��]�A�g�嗦������ł��܂�
	/// [�����P] �I�u�W�F�N�g�̌�����(���_�⃋�[�g�V�O�l�`���Ȃ�)�̃\�[�X�ł�
	/// [�����Q] ���[���h���W
	//----------------------------------------------------------------------------------------------------
	GameObject(
		std::shared_ptr<class ObjectResource> model,
		std::unique_ptr<class WorldTransform> world
	);
	virtual ~GameObject();

	//---------------------------------------------------
	/// [�@�\] ���̃I�u�W�F�N�g�̍X�V������
	//---------------------------------------------------
	void UpdateGameObject();

	//---------------------------------------------------
	///[�@�\] �I�u�W�F�N�g��`�悵�܂�
	///[����] �J����
	//---------------------------------------------------
	virtual void Render(const class Camera& camera); 


	//---------------------------------------------------
	/// [�@�\] �e���e�N�X�`���Ƀ}�b�v����
	/// [����] �J����
	//---------------------------------------------------
	void ShadowMap(const class Camera& camera);

	//---------------------------------------------------
	/// [�@�\] �C�x���g�𑗐M����
	/// [����] �C�x���g
	//---------------------------------------------------
	virtual void ReceiveEvent(const class Event& event);


	//---------------------------------------------------
	///[�@�\] �I�u�W�F�N�g�������Ă��Ă悢�̂��擾
	///[�߂�l] �I�u�W�F�N�g�������Ă悢�̂�
	//---------------------------------------------------
	bool NeedErase();

	//--------------------------------------------------
	/// [�@�\] �����蔻���Ԃ�
	//--------------------------------------------------
	const class HitBox& GetHitBox();

	//--------------------------------------------------
	/// ���[���h�g�����X�t�H�[���̃Q�b�^�[�Z�b�^�[
	//--------------------------------------------------
	void SetWorld(const class WorldTransform& world);
	const class WorldTransform& GetWorld();
	void SetPos(const DirectX::XMFLOAT3& pos);
	DirectX::XMFLOAT3 GetPos();


protected:
	
	State mState;                                                  // �I�u�W�F�N�g�̏��
	std::shared_ptr<class ObjectResource> mResource;               // �����ڂȂǂ̃��\�[�X
	std::unique_ptr<class WorldTransform> mWorld;                  // ���[���h�g�����X�t�H�[��
	std::unique_ptr<class HitBox> mHitBox;
	
private:

	virtual void Update();
	
	
};

typedef std::shared_ptr<GameObject> GameObjectPtrS;


