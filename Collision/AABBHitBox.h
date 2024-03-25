#pragma once
#include "HitBox.h"

class AABBHitBox : public HitBox{
public:
	AABBHitBox(const DirectX::XMFLOAT3& ownerPos);
	~AABBHitBox()override {};

	bool IsHitWithSphere(const class SphereHitBox& sphere)const override { return true; };

private:

};
