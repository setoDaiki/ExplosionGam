#include "NewScene.h"
#include "ObjectComponent/Model.h"
#include "ObjectComponent/WorldTransform.h"
#include "Manager/ModelManager.h"
#include "AudioSystem.h"
#include "GameObject/GameObject.h"
#include "GameObject/Robot.h"
#include "Input/KeyInput.h"
#include "Camera/TestCamera.h"
#include "Rendering/Renderer.h"
#include "App.h"
#include "Common/MyMath.h"
#include "Scene/Sequence.h"
#include <debugapi.h>

using namespace DirectX;

NewScene::NewScene() 
	:mCamera(new TestCamera(XMFLOAT3(0.0f, 0.5f, -8.5f),XMFLOAT3(0.0f, 0.5f, 0.0f)))
	,mAudioSystem(AudioSystem::GetInstance())
{
	/*constexpr float radianRot = DirectX::XMConvertToRadians(90.0f);
	DirectX::XMVECTOR quaternion = XMQuaternionRotationRollPitchYaw(radianRot,0.0f,0.0f);
	WorldPtrU world(new WorldTransform(XMFLOAT3(0.0f,3.0f,0.0f),XMFLOAT3(1.0f,1.0f,1.0f),quaternion));
	WorldPtrU world3(new WorldTransform(XMFLOAT3(0.0f,0.0f,0.0f),XMFLOAT3(1.0f,1.0f,1.0f)));
	WorldPtrU world2(new WorldTransform(XMFLOAT3(0.0f,-1.0f,0.0f)));
	WorldPtrU world4(new WorldTransform(XMFLOAT3(-2.0f,0.0f,-3.0f)));
	ModelPtrS model3 = ModelManager::GetModel("Asset/RobotA_hello.obj");
	ModelPtrS model4 = ModelManager::GetModel("Asset/test.obj");
	ModelPtrS model5(new Model(L"Asset/Sky_Sphere_R10.obj",true));
	GameObjectPtrS gameObj3(new GameObject(model3, std::move(world)));
	GameObjectPtrS gameObj(new GameObject(model4,std::move(world2)));
	GameObjectPtrS gameObj4(new GameObject(model5,std::move(world3)));
	robPtr gameObj5(new Rob(model3, std::move(world4)));
	mGameObjects.push_back(gameObj);
	mGameObjects.push_back(gameObj3);
	//mGameObjects.push_back(gameObj4);
	mGameObjects.push_back(gameObj5);
	AudioSystem::SoundEvent soundEvent = mAudioSystem.PlayEvent("event:/walking_bgm");
	float volume = soundEvent.GetVolume();
	volume *= 0.1f;
	soundEvent.SetVolume(volume);
	mLisPos = mCamera->GetPos();
	mAudioSystem.SetListenerPos(mLisPos);*/
}

//------------------------------------
//  シーンの更新
//------------------------------------
Scene* NewScene::Update() {
	/*
	static unsigned count = 0;
	count++;
	
	for (GameObjectPtrS obj : mGameObjects) {
		obj->Update();
	}
	KeyInput::UpdateCurrentKey();
	mCamera->Update();
	mLisPos = mCamera->GetPos();
	mAudioSystem.SetListenerPos(mLisPos);
	mAudioSystem.Update();
	std::erase_if(mGameObjects, [](GameObjectPtrS object) {return object->NeedErase() == true; });*/
	return this;
}


//--------------------------------
//  シーンの描画
//--------------------------------
void NewScene::Render() {
	/*Renderer::BeginRender();
	for (GameObjectPtrS obj : mGameObjects) {
		obj->ShadowMap(*mCamera);
	}
	
	for (GameObjectPtrS obj : mGameObjects) {
		obj->Render(*mCamera);
	}
	Renderer::EndRender();
	*/
}