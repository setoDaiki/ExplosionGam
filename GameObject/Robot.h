#pragma once
#include "GameObject.h"
#include <DirectXMath.h>
#include <memory>
 

class Robot : public GameObject {
public:
	Robot();
	~Robot()override;
	void Update()override;
	void Render(const class Camera& camera)override;
	void ReceiveEvent(const class Event& event)override;

private:
	DirectX::XMFLOAT3 mMoveDirection;
	void ReverseMovementX();
	void ReverseMovementZ();
	void BePushedByPlayer(const DirectX::XMFLOAT3& playerPos);
	void AttachBomb();
	void Explosion();
	void Detonate();
	float mRotY;
	const float mRevisedValue;
	bool mBombAttaching;;
	static std::unique_ptr<class Sprite> mBombAttachingNotifier;
};



using RobotPtrS = std::shared_ptr<Robot>;