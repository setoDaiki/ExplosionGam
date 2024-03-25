#pragma once
#include "GameObject.h"

class Explosion : public GameObject {
public:
	Explosion(const DirectX::XMFLOAT3& pos,const DirectX::XMFLOAT3& scale);
	~Explosion()override;

	void Update()override;


private:
	unsigned mLifeSpan;
	unsigned mElapsedTime;
};
