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
//      �R���X�g���N�^�[(�ʒu�A��]�A�g�嗦�A������)�̏�����
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
//     �f�X�g���N�^�\
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
//    3D�I�u�W�F�N�g�̕`��
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
//   �e���}�b�v����
//--------------------------------------------------
void GameObject::ShadowMap(const Camera& camera) {
	if (mState != EActive) return;
	mWorld->UpdateWorldMat();
	//mModel->ShadowMap(mWorld->GetWorldMat(),camera);
}


//---------------------------------------
//   �G�x���g���󂯎��
//---------------------------------------
void GameObject::ReceiveEvent(const Event& event) {

}

//---------------------------------------
//  �I�u�W�F�N�g�������Ă悢�̂��擾
//---------------------------------------
bool GameObject::NeedErase() {
	if(mState == EDead) return true;
	return false;
}

//---------------------------------------
//  �����蔻����҂�
//---------------------------------------
const HitBox& GameObject::GetHitBox() {
	return *mHitBox.get();
}

//-------------------------------------------------
//   ���[���h�g�����X�t�H�[���̃Q�b�^�[�Z�b�^�[
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

