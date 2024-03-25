#include "Explosion.h"
#include "Manager/ModelManager.h"
#include "ObjectComponent/WorldTransform.h"
#include "ObjectComponent/Model.h"
#include "Collision/SphereHitBox.h"
#include "Common/MyMath.h"
#include "AudioSystem.h"
#include <memory>

Explosion::Explosion(const DirectX::XMFLOAT3& pos,const DirectX::XMFLOAT3& scale)
	:GameObject(ModelManager::GetModel("Asset/Explosion2.obj"),WorldPtrU(new WorldTransform(pos,scale)))
	,mElapsedTime(0)
	,mLifeSpan(120)
{
	DirectX::XMFLOAT3 position = mWorld->GetPos();
	position.y = 5.0f;
	mHitBox = std::make_unique<SphereHitBox>(mWorld->GetPos(),0.7f * scale.x);
	AudioSystem& audioSystem = AudioSystem::GetInstance();
	audioSystem.PlayEvent("event:/Explosion");
}


Explosion::~Explosion() {

}


void Explosion::Update() {
	DirectX::XMVECTOR quaternion = Math::QuaternionRotateEuler(0.0f,3.0f,0.0f);
	mWorld->AddRot(quaternion); // ”š”­‚Í‰ñ“]‚³‚¹‚Ä‚¨‚­
	mElapsedTime++;
	// Œo‰ßŽžŠÔ‚ªŽõ–½‚ð’´‚¦‚½‚çŽ€‚Ê
	if (mElapsedTime >= mLifeSpan) {
		mState = State::EDead;
	}
}