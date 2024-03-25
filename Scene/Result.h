#pragma once
#include "Scene.h"
#include <memory>

class Result : public Scene {
public:
	explicit Result(unsigned survivalTime,unsigned destroyMonsterCount);
	~Result();

private:
	void ProcessInput()override;
	NextScene Update()override;
	void Render();

	std::unique_ptr<class Camera> mCamera;
	std::unique_ptr<class Sprite> mResultSprite;
	std::unique_ptr<class WorldTransform> mSpriteWorld;
	// �J�E���g�͉�ʂ̃J�E���^�[���\�����Ă��鐔�B�����������Ă����āA�ŏI�I�ɂ͐����c�莞�Ԃ�A���j���Ŏ~�܂�B
	float mSurvivalCount;
	float mDestroyMonsterCount;
	float mTotalScoreCount;
	// �������̓Q�[���ł̐����c�莞�Ԃ⌂�j���A�g�[�^���X�R�A
	unsigned mTotalScore;
	unsigned mSurvivalTime;
	unsigned mNumDestroyMonster;
	bool mStartDestroyMonsterCounter;
	bool mStartTotalScoreCounter;
};