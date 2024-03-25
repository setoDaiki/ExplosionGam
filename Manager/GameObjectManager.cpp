#include "GameObjectManager.h"
#include "GameObject/GameObject.h"
#include "Camera/Camera.h"
#include "GameObject/Robot.h"
#include "ObjectComponent/Sprite.h"
#include "ObjectComponent/WorldTransform.h"
#include "Common/MyMath.h"
#include "GameObject/Player.h"
#include "Collision/NullHitBox.h"
#include "ObjectComponent/Model.h"
#include "ModelManager.h"
#include "GameObject/Explosion.h"
#include "GameObject/Spawner.h"
#include "GameObject/Monster.h"
#include "GameObject/Missile.h"
#include <algorithm>
#include <iostream>

std::unordered_map<Object::Type, std::vector<GameObjectPtrS>> GameObjectManager::mGameObjects;
std::vector<std::pair<Object::Type, GameObjectPtrS>> GameObjectManager::mPendingGameObjects;

void GameObjectManager::UpdateAll() {
	
	for (auto obj : mPendingGameObjects) {
		RegisterToHash(obj.first,obj.second);
	}
	mPendingGameObjects.clear();

	for (auto& objs : mGameObjects) {
		std::erase_if(objs.second, [](GameObjectPtrS obj) {	return obj->NeedErase() == true; });
	}


	for (auto& objs : mGameObjects) {
		for (auto obj : objs.second) {
			obj->UpdateGameObject();
		}
	}
}



void GameObjectManager::RenderAll(const Camera& camera) {
	
	for (auto objs : mGameObjects) {
		for (auto obj: objs.second) {
			
			obj->Render(camera);
		}
	}
	
}

std::vector<GameObjectPtrS> GameObjectManager::GetGameObject(Object::Type type) {
	auto iter = mGameObjects.find(type);
	if (iter == mGameObjects.end()) {
		std::vector<GameObjectPtrS> emptyVector;
		return emptyVector;
	}
	return iter->second;
}


void GameObjectManager::RemoveGameObject(Object::Type type, GameObjectPtrS gameObj) {
	auto iter = mGameObjects.find(type);
	if (iter == mGameObjects.end()) return;

	std::vector<GameObjectPtrS> gameObjects = iter->second;
	std::erase_if(gameObjects, [gameObj](GameObjectPtrS object) {return object == gameObj; });
	
}

void GameObjectManager::RemoveAll() {
	mGameObjects.clear();
}




void GameObjectManager::RegisterToHash(Object::Type type,GameObjectPtrS gameObject) {
	auto iter = mGameObjects.find(type);

	
	if (iter != mGameObjects.end()) {
		iter->second.push_back(gameObject);
	}
	
	else {
		std::vector<GameObjectPtrS> objects;
		objects.push_back(gameObject);
		mGameObjects.emplace(type, objects);
	}
}

void GameObjectManager::RegisterToPending(Object::Type type,GameObjectPtrS gameObject) {
	TypeAndObj typeAndObj;
	typeAndObj.first = type;
	typeAndObj.second = gameObject;
	mPendingGameObjects.push_back(typeAndObj);
}

GameObjectPtrS GameObjectManager::CreatePlayer() {
	ModelManager::GetModel("Asset/Explosion2.obj");
	ModelManager::GetModel("Asset/robot.fbx");
	ModelManager::GetModel("Asset/shot.obj");
	ModelManager::GetModel("Asset/kuiiji.obj");
	GameObjectPtrS player = std::make_shared<Player>();
	RegisterToPending(Object::EPlayer,player);
	return player;
}


GameObjectPtrS GameObjectManager::CreateRobot() {
	GameObjectPtrS robot = std::make_shared<Robot>();
	RegisterToPending(Object::ERobot,robot);
	return robot;
}


GameObjectPtrS GameObjectManager::CreateExplosion(const XMFLOAT3& pos) {
	GameObjectPtrS explosion(new Explosion(pos,XMFLOAT3(6.0,6.0f,6.0f)));
	RegisterToPending(Object::EExplosion, explosion);
	return explosion;
}

GameObjectPtrS GameObjectManager::CreateBigExplosion(const XMFLOAT3& pos) {
	GameObjectPtrS explosion(new Explosion(pos,XMFLOAT3(10.0f,10.0f,10.0f)));
	RegisterToPending(Object::EBigExplosion,explosion);
	return explosion;
}

GameObjectPtrS GameObjectManager::CreateMonster() {
	GameObjectPtrS monster(new Monster());
	RegisterToPending(Object::EMonster, monster);
	return monster;
}

GameObjectPtrS GameObjectManager::CreateRobotSpawner() {
	GameObjectPtrS robotSpawner(new Spawner(CreateRobot,40));
	RegisterToPending(Object::ERobotSpawner, robotSpawner);
	return robotSpawner;
}

GameObjectPtrS GameObjectManager::CreateMonsterSpawner() {
	GameObjectPtrS monsterSpawner(new Spawner(CreateMonster, 300));
	RegisterToPending(Object::EMonsterSpawner, monsterSpawner);
	return monsterSpawner;
}

GameObjectPtrS GameObjectManager::CreateMissile(const XMFLOAT3& pos, const XMVECTOR& rot) {
	GameObjectPtrS missile(new Missile(pos,rot));
	RegisterToPending(Object::EMissile,missile);
	return missile;
}



GameObjectPtrS GameObjectManager::CreateFloor() {
	ObjectResourcePtrS resource(new Sprite(L"Asset/Floor.png"));
	DirectX::XMFLOAT3 floorPos(0.0f, 5.0f, 0.0f);
	DirectX::XMVECTOR floorRot = Math::QuaternionRotateEuler(270.0f,0.0f,0.0f);
	DirectX::XMFLOAT3 floorScale(50.0f, 50.0f, 50.0f);
	WorldPtrU floorWorld(new WorldTransform(floorPos, floorScale, floorRot));
	GameObjectPtrS gameObj(new GameObject(resource, std::move(floorWorld)));
	RegisterToPending(Object::EFloor,gameObj);
	return gameObj;
}



GameObjectPtrS GameObjectManager::CreateFrontWall() {
	ObjectResourcePtrS resource(new Sprite(L"Asset/Wall.png"));
	DirectX::XMFLOAT3 pos(0.0f, 5.0f, 50.0f);
	DirectX::XMVECTOR rot = Math::QuaternionRotateEuler(0.0f, 180.0f, 0.0f);
	DirectX::XMFLOAT3 scale(50.0f, 50.0f, 50.0f);
	WorldPtrU world(new WorldTransform(pos, scale, rot));
	GameObjectPtrS gameObj(new GameObject(resource, std::move(world)));
	RegisterToPending(Object::EFloor, gameObj);
	return gameObj;
}

GameObjectPtrS GameObjectManager::CreateRightWall() {
	ObjectResourcePtrS resource(new Sprite(L"Asset/Wall.png"));
	DirectX::XMFLOAT3 pos(50.0f, 5.0f, 0.0f);
	DirectX::XMVECTOR rot = Math::QuaternionRotateEuler(0.0f, 270.0f, 0.0f);
	DirectX::XMFLOAT3 scale(50.5f, 50.0f, 50.0f);
	WorldPtrU world(new WorldTransform(pos, scale, rot));
	GameObjectPtrS gameObj(new GameObject(resource, std::move(world)));
	RegisterToPending(Object::ERightWall, gameObj);
	return gameObj;
}

GameObjectPtrS GameObjectManager::CreateLeftWall() {
	ObjectResourcePtrS resource(new Sprite(L"Asset/Wall.png"));
	DirectX::XMFLOAT3 pos(-50.0f, 5.0f, 0.0f);
	DirectX::XMVECTOR rot = Math::QuaternionRotateEuler(0.0f, 90.0f, 0.0f);
	DirectX::XMFLOAT3 scale(50.0f, 50.0f, 50.0f);
	WorldPtrU world(new WorldTransform(pos, scale, rot));
	GameObjectPtrS gameObj(new GameObject(resource, std::move(world)));
	RegisterToPending(Object::ELeftWall, gameObj);
	return gameObj;
}