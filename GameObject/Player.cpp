#include "Player.h"
#include "Manager/ModelManager.h"
#include "Manager/GameObjectManager.h"
#include "ObjectComponent/WorldTransform.h"
#include "ObjectComponent/Model.h"
#include "ObjectComponent/UI.h"
#include "ObjectComponent/HitPoint.h"
#include "ObjectComponent/InvincibleTime.h"
#include "ObjectComponent/InputComponent.h"
#include "Input/KeyBoardCommand.h"
#include "Input/GamepadButtonCommand.h"
#include "Input/GamepadStickCommand.h"
#include "Input/KeyInput.h"
#include "Collision/SphereHitBox.h"
#include "Collision/Collider.h"
#include "Common/MyMath.h"
#include "Event.h"
#include "AudioSystem.h"
#include "Camera/Camera.h"
#include <iostream>

using namespace DirectX;

Player::Player()
	:GameObject(ModelPtrS(ModelManager::GetModel("Asset/boy.obj")), WorldPtrU(new WorldTransform(XMFLOAT3(0.0f, 5.0f, 0.0f))))
	, mHitPoint(new HitPoint(5,120))
	, mInput(std::make_unique<InputComponent>())
	, mDestroyMonsterCount(0)
{
	XMVECTOR quaternion = Math::QuaternionRotateEuler(0.0f, 180.0f, 0.0f);
	mWorld->SetRot(quaternion);
	mWorld->SetScale(XMFLOAT3(6.f, 6.f, 6.f));
	mCanAttachBombRange = std::make_unique<SphereHitBox>(mWorld->GetPos(), 5.f);
	mHitBox = std::make_unique<SphereHitBox>(mWorld->GetPos(), 2.0f);


	auto moveRight = [this]()
	{
		mWorld->AddPos(XMFLOAT3(0.5f, 0.0f, 0.0f));
		mWorld->SetRot(Math::QuaternionRotateEuler(0.0f, 180.0f, 0.0f));
		if (Collider::IsHitWithWall(mWorld->GetPos())) {
			mWorld->AddPos(XMFLOAT3(-0.5f, 0.0f, 0.0f));
		}
	};
	auto moveLeft = [this]()
	{
		mWorld->AddPos(XMFLOAT3(-0.5f, 0.0f, 0.0f));
		mWorld->SetRot(Math::QuaternionRotateEuler(0.0f, 0.0f, 0.0f));
		if (Collider::IsHitWithWall(mWorld->GetPos())) {
			mWorld->AddPos(XMFLOAT3(0.5f, 0.0f, 0.0f));
		}
	};
	auto moveFront = [this]()
	{
		mWorld->AddPos(XMFLOAT3(0.0f, 0.0f, 0.5f));
		mWorld->SetRot(Math::QuaternionRotateEuler(0.0f, 90.0f, 0.0f));
		if (Collider::IsHitWithWall(mWorld->GetPos())) {
			mWorld->AddPos(XMFLOAT3(0.0f, 0.0f, -0.5f));
		}
	};
	auto moveBack = [this]()
	{
		mWorld->AddPos(XMFLOAT3(0.0f, 0.0f, -0.5f));
		mWorld->SetRot(Math::QuaternionRotateEuler(0.0f, 270.0f, 0.0f));
		if (Collider::IsHitWithWall(mWorld->GetPos())) {
			mWorld->AddPos(XMFLOAT3(0.0f, 0.0f, 0.5f));
		}
	};

	// �E�ړ��R�}���h��ǉ�
	KeyBoardInput moveRightInput{ 'D',EHold };
	CommandPtrU moveRightCommand = std::make_unique<KeyBoardCommand>(moveRight, moveRightInput);
	mInput->AddCommand(std::move(moveRightCommand));

	// ���ړ��R�}���h��ǉ�
	KeyBoardInput moveLeftInput{ 'A',EHold };
	CommandPtrU moveLeftCommand = std::make_unique<KeyBoardCommand>(moveLeft, moveLeftInput);
	mInput->AddCommand(std::move(moveLeftCommand));

	// �O���ړ��R�}���h��ǉ�
	KeyBoardInput moveFrontInput{ 'W',EHold };
	CommandPtrU moveFrontCommand = std::make_unique<KeyBoardCommand>(moveFront, moveFrontInput);
	mInput->AddCommand(std::move(moveFrontCommand));

	// ����ړ��R�}���h��ǉ�
	KeyBoardInput moveBackInput{ 'S',EHold };
	CommandPtrU moveBackCommand = std::make_unique<KeyBoardCommand>(moveBack, moveBackInput);
	mInput->AddCommand(std::move(moveBackCommand));

	auto moveByStick = [this](XMFLOAT2 stickAxis)
	{
		XMFLOAT3 moveVec(stickAxis.x, 0.0f, stickAxis.y);
		mWorld->AddPos(moveVec);
		if (Collider::IsHitWithWall(mWorld->GetPos())) {
			mWorld->AddPos(XMFLOAT3(-stickAxis.x, 0.0f, -stickAxis.y));
		}
	};
	// �X�e�B�b�N�ړ��R�}���h�ǉ�
	CommandPtrU moveCommand = std::make_unique<GamepadStickCommand>(moveByStick, ERStick);
	mInput->AddCommand(std::move(moveCommand));


	auto AttachBomb = [this]() 
	{
		std::vector<GameObjectPtrS> robots = Collider::GetHitObjects(Object::ERobot, *mCanAttachBombRange.get());
		for (auto robot : robots) {
			Event event;
			event.mType = Event::Type::EAttachBomb;
			robot->ReceiveEvent(event);
		}
	};

	KeyBoardInput attachBombKeyCommand{VK_LBUTTON,EPush};
	GamepadButtonInput attachBombButtonCommand{ XINPUT_GAMEPAD_B,EPush };
	mInput->AddCommand(std::make_unique<GamepadButtonCommand>(AttachBomb, attachBombButtonCommand));
	mInput->AddCommand(std::make_unique<KeyBoardCommand>(AttachBomb,attachBombKeyCommand));

	auto Detonate = []() 
	{
		std::vector<GameObjectPtrS> robots = GameObjectManager::GetGameObject(Object::ERobot);
		AudioSystem& audioSystem = AudioSystem::GetInstance();
		AudioSystem::SoundEvent sound = audioSystem.PlayEvent("event:/Detonate");
		float volume = sound.GetVolume();
		volume *= 0.25f;
		sound.SetVolume(volume);
		for (auto robot : robots) {
			Event event;
			event.mType = Event::Type::EDetonate;
			robot->ReceiveEvent(event);
		}
	};

	KeyBoardInput detonateRobKeyCommand{ VK_RBUTTON,EPush };
	GamepadButtonInput detonateRobButtonCommand{ XINPUT_GAMEPAD_A,EPush };
	mInput->AddCommand(std::make_unique<GamepadButtonCommand>(Detonate,detonateRobButtonCommand));
	mInput->AddCommand(std::make_unique<KeyBoardCommand>(Detonate,detonateRobKeyCommand));
	
}

Player::~Player() {
	
}


void Player::Update() {
	mInput->Update();

	
	// ���G�Ȃ�A��
	if (mHitPoint->IsInvincible()) {
	    mHitPoint->Update();
		return;  // ���̌�̓_���[�W���W�b�N�����疳�G�Ȃ̂ŋA��
	}

	const bool IsHitToExplosion = Collider::IsHit(Object::EExplosion, static_cast<const SphereHitBox&>(GetHitBox()));
	const bool IsHitToMonster = Collider::IsHit(Object::EMonster, static_cast<const SphereHitBox&>(GetHitBox()));
	const bool IsHitToMissile = Collider::IsHit(Object::EMissile, static_cast<const SphereHitBox&>(GetHitBox()));
	const bool IsHitToBigExplosion = Collider::IsHit(Object::EBigExplosion, static_cast<const SphereHitBox&>(GetHitBox()));
	const bool IsDamege = (IsHitToExplosion) || (IsHitToMonster) || (IsHitToMissile) || (IsHitToBigExplosion);
	// �_���[�W��H�������
	if (IsDamege) {
		AudioSystem& audioSystem = AudioSystem::GetInstance();
		audioSystem.PlayEvent("event:/Damege"); // �_���[�WSE���Ȃ炵��
		IsDead isDead = mHitPoint->Damege(1);   // HP��1���炷
		// �_���[�W��H����Ď��񂾂Ȃ�
		if (isDead) {
			mState = State::EDead;  // ����
		}
	}

}

//------------------------------
//  �C�x���g
//------------------------------
void Player::ReceiveEvent(const Event& event) {
	switch (event.mType) {
	case (Event::Type::EDestroyedByPlayer):  // �l�ɂ���ă��{�b�g�����U�����Ȃ�
		mDestroyMonsterCount++;              // ���U���������{�b�g�J�E���g�𑝂₷�B
		break;
	default:
		break;
	}
}

//------------------------------
//   �`��
//------------------------------
void Player::Render(const Camera& camera) {
	GameObject::Render(camera);
	mHitPoint->RenderHitPoint(camera);
}

//----------------------------------
//   ���j�����X�^�[�����擾
//----------------------------------
unsigned Player::GetDesctroyMonsterCount() {
	return mDestroyMonsterCount;
}

