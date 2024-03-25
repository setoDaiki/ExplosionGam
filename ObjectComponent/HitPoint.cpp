#include "HitPoint.h"
#include "WorldTransform.h"
#include "UI.h"
#include "Camera/Camera.h"
#include "Common/MyMath.h"
#include "InvincibleTime.h"

HitPoint::HitPoint(unsigned maxHitPoint,int invincibleTime) 
	:mMaxHP(maxHitPoint)
	,mCurrentHP(maxHitPoint)
	,mHitPointUI(new UI(L"Asset/Heart.png"))
	,mHitPointUIWorld(new WorldTransform(DirectX::XMFLOAT3(-0.95f, 0.95f, 0.2f), DirectX::XMFLOAT3(0.05f, 0.05f, 0.05f)))
	,mInvincible(std::make_unique<InvincibleTime>(invincibleTime))
{
	DirectX::XMVECTOR hitPointUIQuaternion = Math::QuaternionRotateEuler(0.0f, 180.0f, 0.0f);
	mHitPointUIWorld->SetRot(hitPointUIQuaternion);
	mHitPointUIWorld->UpdateWorldMat();
}

void HitPoint::Update() {
	mInvincible->Update();
}


IsDead HitPoint::Damege(unsigned amount) {
	if (mInvincible->IsInvincible()) return false; // –³“G‚È‚ç‹A‚é
	mCurrentHP = mCurrentHP - amount;
	if (mCurrentHP <= 0) {
		return true;
	}
	mInvincible->Start();
	return false;
}


void HitPoint::Recovery(unsigned amount) {
	mCurrentHP += amount;
	if (mCurrentHP >= mMaxHP) {
		mCurrentHP = mMaxHP;
	}
}

bool HitPoint::IsInvincible() {
	if (mInvincible->IsInvincible()) return true;
	return false;
}


void HitPoint::RenderHitPoint(const Camera& camera) {
	for (unsigned i = 0; i < mCurrentHP; i++) {
		mHitPointUI->Render(mHitPointUIWorld->GetWorldMat(), camera);
		mHitPointUIWorld->AddPos(DirectX::XMFLOAT3(0.08f, 0.0f, 0.0f));
		mHitPointUIWorld->UpdateWorldMat();
	}
	mHitPointUIWorld->SetPos(DirectX::XMFLOAT3(-0.95f, 0.95f, 0.2f));
	mHitPointUIWorld->UpdateWorldMat();
}