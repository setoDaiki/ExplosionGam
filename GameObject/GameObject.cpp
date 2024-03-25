#include "GameObject.h"
#include <iostream>
#include <Windows.h>
#include "ObjectComponent/ObjectResource.h"
#include "ObjectComponent/WorldTransform.h"
#include "Common/MyMath.h"
#include "Camera/Camera.h"
#include "Event.h"
#include "Collision/NullHitBox.h"

using namespace DirectX;


//----------------------------------------------------------------
//      コンストラクター(位置、回転、拡大率、見た目)の初期化
//----------------------------------------------------------------
GameObject::GameObject(
	ObjectResourcePtrS resource,
	WorldPtrU world
	)
	:mResource(resource)
	,mState(EActive)
	,mWorld(std::move(world))
	,mHitBox(new NullHitBox())
{

	
}


//---------------------------------------------------------
//     デストラクタ―
//---------------------------------------------------------
GameObject::~GameObject() {

}

void GameObject::UpdateGameObject() {
	if (mState != EActive) return;
	Update();
	mWorld->UpdateWorldMat();
}

void GameObject::Update() {
	
}

//-------------------------------------------------
//    3Dオブジェクトの描画
//-------------------------------------------------
void GameObject::Render(const Camera& camera)
{
	ProjectionComponents projComp = camera.GetProjectionComponents();
	//if (!VFCulling(mWorld->GetPos(),camera.GetViewMat(),projComp.fov,projComp.nearClip,projComp.farClip,projComp.aspect)) {
	//	mState = EStopUndraw;
	//	return;
	//}

	mResource->Render(mWorld->GetWorldMat(), camera);
}


//--------------------------------------------------
//   影をマップする
//--------------------------------------------------
void GameObject::ShadowMap(const Camera& camera) {
	if (mState != EActive) return;
	mWorld->UpdateWorldMat();
	//mModel->ShadowMap(mWorld->GetWorldMat(),camera);
}


//---------------------------------------
//   エベントを受け取る
//---------------------------------------
void GameObject::ReceiveEvent(const Event& event) {

}

//---------------------------------------
//  オブジェクトを消してよいのか取得
//---------------------------------------
bool GameObject::NeedErase() {
	if(mState == EDead) return true;
	return false;
}

//---------------------------------------
//  当たり判定を還す
//---------------------------------------
const HitBox& GameObject::GetHitBox() {
	return *mHitBox.get();
}

//-------------------------------------------------
//   ワールドトランスフォームのゲッターセッター
//-------------------------------------------------
void GameObject::SetWorld(const WorldTransform& world) {
	*mWorld = world;
}

const WorldTransform& GameObject::GetWorld() {
	return *mWorld.get();
}

void GameObject::SetPos(const XMFLOAT3& pos) {
	mWorld->SetPos(pos);
}

XMFLOAT3 GameObject::GetPos() {
	return mWorld->GetPos();
}

