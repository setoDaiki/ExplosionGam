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
	mSurvivalCount += 0.5f; // ��ʂɕ\�����Ă���J�E���g�𑝂₷�B
	// �J�E���g�������c�莞�Ԃ��傫���Ȃ�
	if (mSurvivalCount >= mSurvivalTime) {
		mSurvivalCount = static_cast<float>(mSurvivalTime); // �J�E���g�𐶂��c�莞�Ԃɂ���
		mStartDestroyMonsterCounter = true;                 // �����c�莞�ԃJ�E���g�������؂����̂ŁA���͌��j�����X�^�[�J�E���g��
	}
	// ���j�����X�^�[�J�E���g��
	if (mStartDestroyMonsterCounter) {
		mDestroyMonsterCount += 0.2f; // ���j���J�E���g�𑝂₵��
		// ���j���J�E���g�����ۂ̌��j���𒴂�����
		if (mDestroyMonsterCount >= mNumDestroyMonster) { 
			mDestroyMonsterCount = static_cast<float>(mNumDestroyMonster); // ���j���J�E���g�����ۂ̌��j���ɂ���
			mStartTotalScoreCounter = true;                                // �g�[�^���X�R�A���J�E���g���n�߂�
		}
	}
	// �g�[�^���X�R�A�J�E���g���Ȃ�
	if (mStartTotalScoreCounter) {
		mTotalScoreCount += 5.0f; // �g�[�^���X�R�A�J�E���g�𑝂₵
		// �g�[�^���X�R�A�J�E���g�����ۂ̃g�[�^���X�R�A�𒴂�����
		if (mTotalScoreCount >= mTotalScore) {
			mTotalScoreCount = static_cast<float>(mTotalScore); // �g�[�^���X�R�A�J�E���g�����ۂ̃X�R�A�ɂ���
			counterSound.Stop(); // �J�E���^�[�����X�g�b�v
			audio.Update();
			Sleep(3000);         // 3�b�~�܂���
			return NextScene(new ScoreRanking(mTotalScore),true); // �X�R�A�����L���O��
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