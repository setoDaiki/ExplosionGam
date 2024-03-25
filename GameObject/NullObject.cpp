#include "NullObject.h"
#include "ObjectComponent/Model.h"
#include "ObjectComponent/WorldTransform.h"
#include "Manager/GameObjectManager.h"
#include "Camera/Camera.h"
#include "Collision/HitBox.h"

NullObject::NullObject()
	:GameObject(ModelPtrS(nullptr),WorldPtrU())
{
	mState = EStopUndraw;
}


void NullObject::Render(const Camera& camera) {

}