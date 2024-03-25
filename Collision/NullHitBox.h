#pragma once
#include "HitBox.h"

class NullHitBox : public HitBox {
public:
	NullHitBox();
	~NullHitBox()override {

	}

	bool IsHitWithSphere(const class SphereHitBox& sphere)const override;

};