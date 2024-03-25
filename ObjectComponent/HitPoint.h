#pragma once
#include <memory>

using IsDead = bool;

class HitPoint {
public:
	

	HitPoint(unsigned maxHitPoint,int invincibleTime = 0);
	
	void Update();
	void Recovery(unsigned amount);
	IsDead Damege(unsigned amount);
	void RenderHitPoint(const class Camera& camera);
	bool IsInvincible();

private:
	const unsigned mMaxHP;
	unsigned mCurrentHP;
	std::unique_ptr<class UI> mHitPointUI;
	std::unique_ptr<class WorldTransform> mHitPointUIWorld;
	std::unique_ptr<class InvincibleTime> mInvincible;

};
