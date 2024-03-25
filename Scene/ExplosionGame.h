#pragma once
#include "Scene.h"
#include <memory>

class ExplosionGame : public Scene {
public:
	ExplosionGame();
	~ExplosionGame()override;

private:
	void ProcessInput()override;
	struct NextScene Update()override;
	void Render()override;

	std::unique_ptr<class Camera> mCamera;
	std::unique_ptr<class Timer> mTimer;
	unsigned mDestroyMonsterCount;
	class SoundEvent* mBGMSound;
};
