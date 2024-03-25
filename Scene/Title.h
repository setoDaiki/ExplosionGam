#pragma once
#include "Scene.h"
#include "AudioSystem.h"
#include <memory>

class Title : public Scene {
public:
	Title();
	~Title()override;


private:
	void ProcessInput()override;
	struct NextScene Update()override;
	void Render()override;

	bool IsPushedStartButton();
	bool IsPushedHowToPlay();
	bool IsCursorRangeInStartButton();
	bool IsCursorRangeInHowToPlay();
	bool mStateHowToPlay;
	short mHowToPlayPageNumber;
	std::unique_ptr<class Sprite> mTitleSprite;
	std::unique_ptr<class Sprite> mHowToPlaySprite;
	std::unique_ptr<class WorldTransform> mHTPWorld;
	std::unique_ptr<class WorldTransform> mWorld;
	std::unique_ptr<class Camera> mCamera;
	AudioSystem::SoundEvent mBGMSound;
};