#pragma once
#include"GameObject.h"

class Monster : public GameObject {
public:
	Monster();

	void Update()override;

private:
	int mCount;
	int mMissileInterval;
	std::unique_ptr<class RotationMovement> mMovement;
};
