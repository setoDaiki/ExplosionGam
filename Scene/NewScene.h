#pragma once
#include "Scene.h"
#include <vector>
#include <memory>
#include <DirectXMath.h>

class NewScene :public Scene{
public:
	NewScene();

private:
	void ProcessInput()override{}
	Scene* Update()override;
	void Render()override;
	
	DirectX::XMFLOAT3 mLisPos;
	std::vector<std::shared_ptr<class GameObject>> mGameObjects;
	std::unique_ptr<class Camera> mCamera;
	std::unique_ptr<class WorldTransform> world;
	class AudioSystem& mAudioSystem;
};

