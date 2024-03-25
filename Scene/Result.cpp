#include "Result.h"
#include "Sequence.h"
#include "Camera/Camera.h"
#include "Camera/StaticCamera.h"
#include "ObjectComponent/WorldTransform.h"
#include "ObjectComponent/Sprite.h"
#include "ObjectComponent/WorldTransform.h"
#include "Number.h"
#include "Rendering/Renderer.h"
#include "Common/MyMath.h"
#include "AudioSystem.h"
#include "Scene/ScoreRanking.h"
#include <DirectXMath.h>


Result::Result(unsigned survivalTime, unsigned destroyMonsterCount)
	: mResultSprite(new Sprite(L"Asset/Result.png"))
	, mCamera(new Camera(DirectX::XMFLOAT3(0.0f, 0.0f, -2.0f), DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), std::make_unique<StaticCamera>()))
	,mSurvivalTime(survivalTime)
	,mNumDestroyMonster(destroyMonsterCount)
	,mDestroyMonsterCount(0.0f)
	,mSurvivalCount(0.0f)
	,mTotalScoreCount(0.0f)
	,mStartDestroyMonsterCounter(false)
	,mStartTotalScoreCounter(false)
{
	DirectX::XMVECTOR spriteQuaternion = Math::QuaternionRotateEuler(0.0f, 180.0f, 0.0f);
	mSpriteWorld = std::make_unique<WorldTransform>(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), DirectX::XMFLOAT3(1.5f, 1.5f, 1.5f),spriteQuaternion);
	mTotalScore = static_cast<unsigned>(static_cast<float>(mNumDestroyMonster * mSurvivalTime) * 0.5f);
}

Result::~Result() {

}


void Result::ProcessInput() {

}

NextScene Result::Update() {
	AudioSystem& audio = AudioSystem::GetInstance();
	static AudioSystem::SoundEvent counterSound = audio.PlayEvent("event:/Counter");
	audio.Update();
	mSurvivalCount += 0.5f; // 画面に表示しているカウントを増やす。
	// カウントが生き残り時間より大きいなら
	if (mSurvivalCount >= mSurvivalTime) {
		mSurvivalCount = static_cast<float>(mSurvivalTime); // カウントを生き残り時間にする
		mStartDestroyMonsterCounter = true;                 // 生き残り時間カウントが増え切ったので、次は撃破モンスターカウントへ
	}
	// 撃破モンスターカウント中
	if (mStartDestroyMonsterCounter) {
		mDestroyMonsterCount += 0.2f; // 撃破数カウントを増やして
		// 撃破数カウントが実際の撃破数を超えたら
		if (mDestroyMonsterCount >= mNumDestroyMonster) { 
			mDestroyMonsterCount = static_cast<float>(mNumDestroyMonster); // 撃破数カウントを実際の撃破数にして
			mStartTotalScoreCounter = true;                                // トータルスコアをカウントし始める
		}
	}
	// トータルスコアカウント中なら
	if (mStartTotalScoreCounter) {
		mTotalScoreCount += 5.0f; // トータルスコアカウントを増やし
		// トータルスコアカウントが実際のトータルスコアを超えたら
		if (mTotalScoreCount >= mTotalScore) {
			mTotalScoreCount = static_cast<float>(mTotalScore); // トータルスコアカウントを実際のスコアにして
			counterSound.Stop(); // カウンター音をストップ
			audio.Update();
			Sleep(3000);         // 3秒止まって
			return NextScene(new ScoreRanking(mTotalScore),true); // スコアランキングへ
		}
	}
	return NextScene(this,false);
}

void Result::Render() {
	static NumberUI survivalTime(DirectX::XMFLOAT3(0.8f,0.7f,0.2f),DirectX::XMFLOAT3(0.3f,0.3f,0.3f));
	static NumberUI destroyCount(DirectX::XMFLOAT3(0.8f,0.0f,0.2f),DirectX::XMFLOAT3(0.3f,0.3f,0.3f));
	static NumberUI score(DirectX::XMFLOAT3(0.8f,-0.7f,0.2f),DirectX::XMFLOAT3(0.3f,0.3f,0.3f));
	Renderer::BeginRender();
	mResultSprite->Render(mSpriteWorld->GetWorldMat(), *mCamera.get());
	survivalTime.Render(*mCamera,static_cast<int>(mSurvivalCount));
	destroyCount.Render(*mCamera,static_cast<int>(mDestroyMonsterCount));
	score.Render(*mCamera,static_cast<int>(mTotalScoreCount));
	Renderer::EndRender();
}