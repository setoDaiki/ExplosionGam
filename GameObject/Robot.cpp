#include "Robot.h"
#include "ObjectComponent/WorldTransform.h"
#include "ObjectComponent/Model.h"
#include "Manager/ModelManager.h"
#include "Manager/GameObjectManager.h"
#include "ObjectComponent/Sprite.h"
#include "Common/MyMath.h"
#include "Common/Random.h"
#include "Common/Printer.h"
#include "Collision/SphereHitBox.h"
#include "Collision/Collider.h"
#include "AudioSystem.h"
#include "Event.h"
#include "Camera/Camera.h"
#include <iostream>
#include <cmath>

using namespace DirectX;

std::unique_ptr<Sprite> Robot::mBombAttachingNotifier; 

Robot::Robot()
	:GameObject(ModelPtrS(ModelManager::GetModel("Asset/robot.fbx")),WorldPtrU(new WorldTransform(XMFLOAT3(0.0f,5.1f,0.0f))))
	,mRevisedValue(0.3f)
	,mBombAttaching(false)
{
	XMFLOAT3 pos = mWorld->GetPos();
	pos.y = 4.0f;
	mWorld->SetPos(pos);
	mWorld->SetScale(XMFLOAT3(0.003f,0.003f,0.003f));
	float moveAngle = Random::RandomFloat(0, 360);
	mRotY = moveAngle;
	XMVECTOR robotRotate = Math::QuaternionRotateEuler(0.0f, moveAngle, 0.0f);
	mWorld->SetRot(robotRotate);
	mMoveDirection = Math::GetFront(robotRotate);
	mMoveDirection = mRevisedValue * mMoveDirection;

	mHitBox = std::make_unique<SphereHitBox>(mWorld->GetPos(), 2.0f);

	static bool isFirst = true;
	if (isFirst) {
		mBombAttachingNotifier = std::make_unique<Sprite>(L"Asset/Bomb.png");
		isFirst = false;
	}
}

Robot::~Robot() {

}



void Robot::Update() {

	// �ǂɓ��������瓮�����]
	if (Collider::IsHitFrontOrBackWall(mWorld->GetPos())) ReverseMovementZ();
	if (Collider::IsHitSideWall(mWorld->GetPos())) ReverseMovementX();
	
	// �v���C���[�ɓ��������牟�����
	std::vector<GameObjectPtrS> gameObject = GameObjectManager::GetGameObject(Object::EPlayer);
	if (!gameObject.empty()) {
		const HitBox& playerHitBox = gameObject[0]->GetHitBox();
		if (playerHitBox.IsHitWithSphere(static_cast<const SphereHitBox&>(GetHitBox()))) {
			BePushedByPlayer(gameObject[0]->GetWorld().GetPos());
			return;  //�����ꂽ�炻�̂܂܋A��
		}
	}

	
	const bool isHitExplosion = Collider::IsHit(Object::EExplosion, static_cast<const SphereHitBox&>(GetHitBox()));
	const bool isHitBigExplosion = Collider::IsHit(Object::EBigExplosion, static_cast<const SphereHitBox&>(GetHitBox()));
	const bool isHitRobot = Collider::IsHit(Object::ERobot, static_cast<const SphereHitBox&>(GetHitBox()));
	const bool explosionFlag = isHitExplosion || isHitBigExplosion || isHitRobot;
	if (explosionFlag) {
		Explosion();
	}
	

	mWorld->AddPos(mMoveDirection);
	
}


void Robot::Render(const Camera& camera) {

	GameObject::Render(camera);
	if (mBombAttaching) {
		
		XMFLOAT3 notifierPos = GetPos();
		notifierPos.y += 10.0f;
		XMVECTOR notifierRot = Math::QuaternionRotateEuler(0.0f,180.0f,0.0f);
		XMFLOAT3 notifierScale(2.0f, 2.0f, 2.0f);
		WorldTransform notifierWorld(notifierPos,notifierScale,notifierRot);
		notifierWorld.UpdateWorldMat();

		mBombAttachingNotifier->Render(notifierWorld.GetWorldMat(), camera);
	}
}


void Robot::ReceiveEvent(const Event& event) {
	switch(event.mType)
	{
	case (Event::Type::EAttachBomb):
		AttachBomb();
		break;

	case (Event::Type::EExplosion):
		Explosion();
		break;
	case (Event::Type::EDetonate):
		Detonate();
	default:
		break;
	}
}

//------------------------------
//   ���e�����t����
//------------------------------
void Robot::AttachBomb() {
	if (!mBombAttaching) {
		AudioSystem& audioSystem = AudioSystem::GetInstance();
		audioSystem.PlayEvent("event:/Attach");
		mBombAttaching = true;
	}
}


//-------------------------------
//   ���U����
//-------------------------------
void Robot::Explosion() {

	GameObjectPtrS explosion = GameObjectManager::CreateExplosion(GetPos());
	mState = State::EDead;

}

//--------------------------------
//  �N������
//--------------------------------
void Robot::Detonate() {
	if (mBombAttaching) {
		GameObjectManager::CreateBigExplosion(GetPos());
		mState = EDead;
	}
}



//----------------------------------------
//  Z�������̈ړ��𔽓]������
//----------------------------------------
void Robot::ReverseMovementZ() {
	mRotY = 360.0f - mRotY;                      // X���ړ����]��̊p�x�� �E�E�E 180��- �O�̊p�x 
	XMVECTOR quaternion = Math::QuaternionRotateEuler(0.0f, mRotY, 0.0f);
	mWorld->SetRot(quaternion);
	mMoveDirection = Math::GetFront(quaternion);
	mMoveDirection = mRevisedValue * mMoveDirection;
}

//------------------------------------------
//   X�������̈ړ��𔽓]������
//------------------------------------------
void Robot::ReverseMovementX() {
	mRotY = 180.0f - mRotY;                      // Z���ړ����]��̊p�x��  �E�E�E 360 - �O�̊p�x
	XMVECTOR quaternion = Math::QuaternionRotateEuler(0.0f, mRotY, 0.0f);
	mWorld->SetRot(quaternion);
	mMoveDirection = Math::GetFront(quaternion);
	mMoveDirection = mRevisedValue * mMoveDirection;
}


//--------------------------------
//   �v���C���[�ɉ������
//--------------------------------
void Robot::BePushedByPlayer(const DirectX::XMFLOAT3& playerPos) {
	DirectX::XMFLOAT3 moveVec = mWorld->GetPos() - playerPos;   // �v���C���[���烍�{�b�g�̕����x�N�g��
	if (std::abs(moveVec.x) > std::abs(moveVec.z)) {
		moveVec.z = 0.0f;
	}
	else {
		moveVec.x = 0.0f;
	}
	moveVec = 0.20f * moveVec;                                  // �ړ��ʂ̒���
	moveVec.y = 0.0f;                                            // y���ړ��͂��Ȃ�
	DirectX::XMFLOAT3 pos = moveVec + mWorld->GetPos();         // �ړ��ʂ����݈ʒu�ɉ�����
	if (Collider::IsHitFrontOrBackWall(pos) || Collider::IsHitSideWall(pos)) {      // ���������ʂǂꂩ�̕ǂɓ�����Ȃ�
		pos = pos - moveVec;						            // �ړ����Ȃ��������Ƃɂ���
	}

	mWorld->SetPos(pos);                                        // �ʒu�̍X�V
}



