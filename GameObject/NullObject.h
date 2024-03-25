#pragma once
#include "GameObject.h"

class NullObject : public GameObject {
public:
	NullObject();
	void Render(const class Camera&)override;


};
