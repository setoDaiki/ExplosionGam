#pragma once
#include "Scene.h"
#include <memory>
#include <array>
#include <vector>

class ScoreRanking : public Scene {
public:
	ScoreRanking(unsigned thisGamesScore);


private:
	void ProcessInput()override;
	struct NextScene Update()override;
	void Render()override;
	std::unique_ptr<class Camera> mCamera;
	std::unique_ptr<class JsonFile> mScoreFile;
	std::unique_ptr<class Sprite> mRankingSprite;
	std::unique_ptr<class UI> mUnderLine;
	std::unique_ptr<class UI> mHighScore;
	std::unique_ptr<class WorldTransform> mRankingWorld;
	std::unique_ptr<class WorldTransform> mUnderLineWorld;
	std::unique_ptr<class WorldTransform> mHighScoreWorld;
	std::vector<unsigned> mScores;
	std::vector<class NumberUI> mScoresUI;
};
