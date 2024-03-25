#pragma once
#include <vector>
#include <unordered_map>
#include <memory>
#include <string>
#include <DirectXMath.h>


namespace Object {
	enum Type {
		EPlayer,
		ERobot,
		EExplosion,
		EBigExplosion,
		EMonster,
		EMissile,
		ERobotSpawner,
		EMonsterSpawner,
		EFloor,
		ERightWall,
		ELeftWall,
		EFrontWall
	};
}

class GameObjectManager {
public:
	
	static std::shared_ptr<class GameObject> CreatePlayer();
	static std::shared_ptr<class GameObject> CreateRobot();
	static std::shared_ptr<class GameObject> CreateMonster();
	static std::shared_ptr<class GameObject> CreateFloor();
	static std::shared_ptr<class GameObject> CreateRightWall();
	static std::shared_ptr<class GameObject> CreateFrontWall();
	static std::shared_ptr<class GameObject> CreateLeftWall();
	static std::shared_ptr<class GameObject> CreateRobotSpawner();
	static std::shared_ptr<class GameObject> CreateMonsterSpawner();
	static std::shared_ptr<class GameObject> CreateMissile(const DirectX::XMFLOAT3& pos,const DirectX::XMVECTOR& rot);
	static std::shared_ptr<class GameObject> CreateExplosion(const DirectX::XMFLOAT3& pos);
	static std::shared_ptr<class GameObject> CreateBigExplosion(const DirectX::XMFLOAT3& pos);

	static void RemoveGameObject(Object::Type type,std::shared_ptr<class GameObject> gameObj);
	static void RemoveAll();

	void AddGameObject();
	static std::vector<std::shared_ptr<class GameObject>> GetGameObject(Object::Type type);

	static void UpdateAll();
	static void RenderAll(const class Camera& camera);

private:
	static void RegisterToHash(Object::Type type,std::shared_ptr<class GameObject> gameObject);
	static void RegisterToPending(Object::Type,std::shared_ptr<class GameObject> gameObject);

	

	static std::unordered_map<Object::Type, std::vector<std::shared_ptr<class GameObject>>> mGameObjects;
	static std::vector<std::pair<Object::Type, std::shared_ptr<class GameObject>>> mPendingGameObjects;

};

using TypeAndObj = std::pair<Object::Type, std::shared_ptr<class GameObject>>;