#pragma once
#include <memory>
#include <vector>
#include <DirectXMath.h>

namespace Object {
	enum Type;
}


class Collider {
public:
	static bool IsHit(Object::Type detectionTarget, const class SphereHitBox& hitBox);
	static std::shared_ptr<class GameObject> GetHitObject(Object::Type detectionTarget,const class SphereHitBox& hitBox);
	static std::vector<std::shared_ptr<class GameObject>> GetHitObjects(Object::Type detectionTarget, const class SphereHitBox& hitBox);
	static bool IsHitWithWall(const DirectX::XMFLOAT3& pos);
	static bool IsHitSideWall(const DirectX::XMFLOAT3& pos);
	static bool IsHitFrontOrBackWall(const DirectX::XMFLOAT3& pos);
};
