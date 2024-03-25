#include "Collider.h"
#include "GameObject/GameObject.h"
#include "Manager/GameObjectManager.h"
#include "SphereHitBox.h"
#include "GameObject/NullObject.h"


namespace {
	GameObjectPtrS nullObj = std::make_shared<NullObject>();
}


bool Collider::IsHit(Object::Type detectionTarget, const SphereHitBox& hitBox) {
	std::vector<GameObjectPtrS> targets = GameObjectManager::GetGameObject(detectionTarget);
	for (auto target : targets) {
		
		const HitBox& targetHitBox = target->GetHitBox();
		if (targetHitBox.IsHitWithSphere(hitBox)) {
			return true;
		}
	}
	return false;
}

GameObjectPtrS Collider::GetHitObject(Object::Type detectionTargt, const SphereHitBox& hitBox) {
	std::vector<GameObjectPtrS> targets = GameObjectManager::GetGameObject(detectionTargt);
	for (auto target : targets) {
		const HitBox& targetHitBox = target->GetHitBox();
		if (targetHitBox.IsHitWithSphere(hitBox)) {
			return target;
		}
	}
	
	return nullObj;
}


std::vector<GameObjectPtrS> Collider::GetHitObjects(Object::Type detectionTarget, const SphereHitBox& hitBox) {
	std::vector<GameObjectPtrS> targets = GameObjectManager::GetGameObject(detectionTarget);
	std::vector<GameObjectPtrS> hitObjects;
	for (auto target : targets) {
		const HitBox& targetHitBox = target->GetHitBox();
		if (targetHitBox.IsHitWithSphere(hitBox)) {
			hitObjects.push_back(target);
		}
	}
	return hitObjects;
}

bool Collider::IsHitWithWall(const DirectX::XMFLOAT3& pos) {
	if (IsHitSideWall(pos))        return true;
	if (IsHitFrontOrBackWall(pos)) return true;
	return false;
}

bool Collider::IsHitFrontOrBackWall(const DirectX::XMFLOAT3& pos) {
	if (pos.z >= 47.f || pos.z <= -47.0f) return true;
	return false;
}

bool Collider::IsHitSideWall(const DirectX::XMFLOAT3& pos) {
	if (pos.x >= 47.f || pos.x <= -47.0f) return true;
	return false;
}