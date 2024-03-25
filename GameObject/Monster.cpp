#include "Monster.h"
#include "Manager/GameObjectManager.h"
#include "Manager/ModelManager.h"
#include "ObjectComponent/WorldTransform.h"
#include "ObjectComponent/Model.h"
#include "ObjectComponent/RotationMovement.h"
#include "Collision/SphereHitBox.h"
#include "Collision/Collider.h"
#include "Common/MyMath.h"
#include "Event.h"

Monster::Monster()
	:GameObject(ModelManager::GetModel("Asset/kuiiji.obj"), WorldPtrU(new WorldTransform()))
	,mMissileInterval(60)
	,mMovement(std::make_unique<RotationMovement>(DirectX::XMVectorSet(0.0f,1.0f,0.0f,0.0f),2.0f))
{
	mWorld->AddPos(DirectX::XMFLOAT3(10.0f,5.0f,0.0f));
	mWorld->SetScale(DirectX::XMFLOAT3(8.0f,8.0f,8.0f));
	mHitBox = std::make_unique<SphereHitBox>(mWorld->GetPos(), 3.5f);
}


void Monster::Update() {

	mWorld->SetRot(mMovement->Update(mWorld->GetRot())); // 回転する
	mCount++;
	// 爆発と当たったら
	if (Collider::IsHit(Object::EExplosion, static_cast<const SphereHitBox&>(GetHitBox()))) {
		mState = EDead; // 死ぬ
		GameObjectManager::CreateExplosion(GetPos()); // そして自分も爆発
	}
	// 巨大な爆発に当たっても
	if (Collider::IsHit(Object::EBigExplosion, static_cast<const SphereHitBox&>(GetHitBox()))) {
		mState = EDead; // 死ぬ
		GameObjectManager::CreateExplosion(GetPos()); // そして自分も爆発
		std::vector<GameObjectPtrS> player = GameObjectManager::GetGameObject(Object::EPlayer);
		if (!player.empty()) {
			Event event;
			// 巨大な爆発はプレイヤーによって発生させられたものなので、プレイヤーによって破壊させたこと伝える
			event.mType = Event::Type::EDestroyedByPlayer;
			player[0]->ReceiveEvent(event);
		}
	}
	// ミサイル発射時間になったらミサイル発射
	if (mCount >= mMissileInterval) {
		DirectX::XMVECTOR rot = mWorld->GetRot();
		GameObjectManager::CreateMissile(GetPos(), rot);
		DirectX::XMVECTOR back = Math::QuaternionRotateEulerAndMultiply(0.0f, 180.0f, 0.0f, rot);
		GameObjectManager::CreateMissile(GetPos(), back); // ミサイル生成

		mCount = 0;
	}

}