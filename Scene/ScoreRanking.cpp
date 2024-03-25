#include "ScoreRanking.h"
#include "Sequence.h"
#include "Rendering/Renderer.h"
#include "Json/JsonFile.h"
#include "AudioSystem.h"
#include "ObjectComponent/Sprite.h"
#include "ObjectComponent/WorldTransform.h"
#include "ObjectComponent/UI.h"
#include "Common/MyMath.h"
#include "Camera/Camera.h"
#include "Number.h"
#include "Camera/StaticCamera.h"
#include "AudioSystem.h"
#include "Input/KeyInput.h"
#include "ExplosionGame.h"
#include <iostream>

ScoreRanking::ScoreRanking(unsigned thisGamesScore)
	:mScoreFile(new JsonFile("Json/Score.json"))
	,mRankingSprite(new Sprite(L"Asset/ScoreRanking.png"))
	,mHighScore(nullptr)
	,mHighScoreWorld(nullptr)
	,mUnderLine(nullptr)
	,mUnderLineWorld(nullptr)
	, mCamera(new Camera(DirectX::XMFLOAT3(0.0f, 0.0f, -2.0f), DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),std::make_unique<StaticCamera>()))
{
	DirectX::XMVECTOR spriteQuaternion = Math::QuaternionRotateEuler(0.0f, 180.0f, 0.0f);
	mRankingWorld = std::make_unique<WorldTransform>(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), DirectX::XMFLOAT3(1.5f, 1.5f, 1.5f), spriteQuaternion);
	mRankingWorld->UpdateWorldMat();

	auto scores = mScoreFile->GetArray("scores");
	mScores.reserve(5);
	mScoresUI.reserve(5);
	// 歴代スコアを取得。UIも用意
	for (unsigned i = 0; i < scores.Size(); i++) {
		float dy = static_cast<float>(i) * 0.4f;
		mScores.emplace_back(scores[i].GetInt());
		mScoresUI.emplace_back(DirectX::XMFLOAT3(0.0f, 0.9f - dy, 0.2f), DirectX::XMFLOAT3(0.1f, 0.1f, 0.1f));
	}

	bool isRankIn = false;
	unsigned rankInIndex = 0;
	// スコアファイルを更新、今回のゲームの点数も考慮する
	for (int i = 0; i < mScores.size(); i++) {
		// 今回の点数の方が大きかったら挿入して、最下位の点数をポップする
		if (thisGamesScore > mScores[i]) {
			scores[i].SetInt(thisGamesScore);
			mScores.insert(mScores.begin()+i,thisGamesScore);
			mScores.pop_back();
			isRankIn = true;
			rankInIndex = i;
			break;
		}
	}

	// ランク入りしたなら更新した記録をアンダーラインで強調
	if (isRankIn) {
		mUnderLine = std::make_unique<UI>(L"Asset/UnderLine.png");
		float dy = static_cast<float>(rankInIndex) * 0.4f + 0.1f;
		DirectX::XMVECTOR quaternion = Math::QuaternionRotateEuler(0.0f, 180.0f, 0.0f);
		mUnderLineWorld = std::make_unique<WorldTransform>(DirectX::XMFLOAT3(0.0f,0.9f - dy, 0.2f),DirectX::XMFLOAT3(0.5f,0.1f,0.1f),quaternion);
		mUnderLineWorld->UpdateWorldMat();
		mHighScore = std::make_unique<UI>(L"Asset/HighScore.jpg");
		mHighScoreWorld = std::make_unique<WorldTransform>(DirectX::XMFLOAT3(0.6f,0.7f,0.2f),DirectX::XMFLOAT3(0.3f,0.3f,0.3f),quaternion);
		AudioSystem::GetInstance().PlayEvent("event:/HighScore");
	}

	mScoreFile->SaveFile();
}


void ScoreRanking::ProcessInput() {
	KeyInput::UpdateCurrentKey();
}

NextScene ScoreRanking::Update() {
	static AudioSystem& audioSystem = AudioSystem::GetInstance();
	audioSystem.Update();
	if (KeyInput::GetKeyState('R') == EPush) {
		return NextScene(new ExplosionGame{},true);
	}
	return NextScene(this,false);
}

void ScoreRanking::Render() {
	Renderer::BeginRender();
	int i = 0;
	for (auto su : mScoresUI) {
		su.Render(*mCamera.get(),mScores[i]);
		i++;
	}
	if (mUnderLine) {
		mUnderLine->Render(mUnderLineWorld->GetWorldMat(), *mCamera.get());
	}
	if (mHighScore) {
		mHighScore->Render(mHighScoreWorld->GetWorldMat(),*mCamera.get());
	}
	mRankingSprite->Render(mRankingWorld->GetWorldMat(), *mCamera.get());
	Renderer::EndRender();
}