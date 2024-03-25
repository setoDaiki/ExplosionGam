#include "Missile.h"
#include "Manager/ModelManager.h"
#include "ObjectComponent/WorldTransform.h"
#include "ObjectComponent/Model.h"
#include "Manager/GameObjectManager.h"
#include "Common/MyMath.h"
#include "Common/Random.h"
#include "Collision/SphereHitBox.h"
#include "Collision/Collider.h"

Missile::Missile(const DirectX::XMFLOAT3& pos ,const DirectX::XMVECTOR& quaternion)
	:GameObject(ModelManager::GetModel("Asset/shot.obj"),WorldPtrU(new WorldTransform(pos,DirectX::XMFLOAT3(6.0f,6.0f,6.0f), quaternion)))
	,mCount(0)
{
	mMoveDirection = Math::GetFront(quaternion);
	mHitBox = std::make_unique<SphereHitBox>(mWorld->GetPos(), 2.0f);

}


void Missile::Update() {
	mWorld->AddPos(mMoveDirection);
	mCount++;
	if (Collider::IsHit(Object::ERobot,static_cast<const SphereHitBox&>(GetHitBox()))) {
		mState = State::EDead;
	}
	if (mCount >=180) { // 180ÉtÉåÅ[ÉÄê∂Ç´écÇ¡ÇΩÇÁéÄÇ 
		mState = State::EDead;
	}

}