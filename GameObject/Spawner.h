#pragma once
#include "GameObject.h"
#include <memory>

namespace Object {
	enum Type;
}

using SpawnFunc = GameObjectPtrS(*)();

class Spawner : public GameObject{
public:
	enum Color {
		Red,
		Blue
	};

	Spawner(SpawnFunc spawnFunc, unsigned spawnCycle);
	~Spawner()override {};

	void Update()override;

private:
	void DecideSpawnPos();

	SpawnFunc mSpawnFunc;
	unsigned mSpawnCycle;
	unsigned mCount;

};
