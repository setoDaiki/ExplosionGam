#pragma once
#include "GameObject.h"
#include <array>
#include <memory>
#include <Windows.h>

class Player : public GameObject {
public:
	Player();
	~Player()override;

	void Update()override;
	void Render(const class Camera& camera)override;
	void ReceiveEvent(const class Event& event)override;
	unsigned GetDesctroyMonsterCount();

private:

	std::unique_ptr<class HitPoint> mHitPoint;
	std::unique_ptr<class SphereHitBox> mCanAttachBombRange;
	std::unique_ptr<class InputComponent> mInput;
	unsigned mDestroyMonsterCount;
	
};
