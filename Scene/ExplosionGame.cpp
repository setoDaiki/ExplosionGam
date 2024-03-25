#include "ExplosionGame.h"
#include "Camera/TestCamera.h"
#include "Manager/GameObjectManager.h"
#include "Rendering/Renderer.h"
#include "Input/KeyInput.h"
#include "Camera/StaticCamera.h"
#include "Camera/Camera.h"
#include "AudioSystem.h"
#include "Scene/Sequence.h"
#include "Scene/Result.h"
#include "ObjectComponent/Timer.h"
#include "GameObject/Player.h"

using namespace DirectX;

ExplosionGame::ExplosionGame()
	:mCamera(new Camera(XMFLOAT3(0.0f, 70.0f, -86.0f), XMFLOAT3(0.0f, 65.0f, -79.0f),std::make_unique<StaticCamera>()))
	,mTimer(new Timer(XMFLOAT3(0.9f,0.9f,0.2f)))
	,mDestroyMonsterCount(0)
{


	GameObjectManager::CreatePlayer();
	GameObjectManager::CreateFloor();
	GameObjectManager::CreateFrontWall();
	GameObjectManager::CreateRightWall();
	GameObjectManager::CreateLeftWall();
	GameObjectManager::CreateRobotSpawner();
	GameObjectManager::CreateMonsterSpawner();
	AudioSystem::GetInstance().PlayEvent("event:/GameBGM");
}


ExplosionGame::~ExplosionGame() {
	GameObjectManager::RemoveAll();
}


void ExplosionGame::ProcessInput() {
	KeyInput::UpdateCurrentKey();
}


NextScene ExplosionGame::Update() {
	AudioSystem& audioSystem = AudioSystem::GetInstance();
	audioSystem.Update();
	mCamera->Update();
	GameObjectManager::UpdateAll();

	// ゲームリトライ
	if (KeyInput::GetKeyState('R') == EPush) {
		Scene* next = new ExplosionGame();
		return NextScene(next,true);
	}

	// プレイヤ―管理コンテナがからならタイマーをストップしてリザルト画面へ
	if (GameObjectManager::GetGameObject(Object::EPlayer).empty()) {
		Scene* next = new Result(mTimer->GetTime(),mDestroyMonsterCount); // リザルト画面をnew
		mTimer->Stop();
		return NextScene(next,true);
	}
	// 終了してない時だけする処理
	else {
		auto gameObject = GameObjectManager::GetGameObject(Object::EPlayer)[0];
		Player* player = static_cast<Player*>(gameObject.get());
		mDestroyMonsterCount = player->GetDesctroyMonsterCount();
	}
	
	return NextScene(this,false);
}


void ExplosionGame::Render() {
	Renderer::BeginRender();
	GameObjectManager::RenderAll(*mCamera);
	mTimer->Render(*mCamera);
	Renderer::EndRender();
}