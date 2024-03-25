#pragma once
#include "GameObject.h"

class Missile : public GameObject {
public:
	Missile(const DirectX::XMFLOAT3& pos ,const DirectX::XMVECTOR& moveDirection);
	~Missile()override {};

	void Update()override;


private:
	int mCount;
	DirectX::XMFLOAT3 mMoveDirection;
};
