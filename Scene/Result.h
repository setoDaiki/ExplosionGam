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
	// カウントは画面のカウンターが表示している数。少しずつ増えていって、最終的には生き残り時間や、撃破数で止まる。
	float mSurvivalCount;
	float mDestroyMonsterCount;
	float mTotalScoreCount;
	// こっちはゲームでの生き残り時間や撃破数、トータルスコア
	unsigned mTotalScore;
	unsigned mSurvivalTime;
	unsigned mNumDestroyMonster;
	bool mStartDestroyMonsterCounter;
	bool mStartTotalScoreCounter;
};