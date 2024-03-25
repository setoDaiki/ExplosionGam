#include "Title.h"
#include "Input/KeyInput.h"
#include "Camera/StaticCamera.h"
#include "ObjectComponent/Sprite.h"
#include "ObjectComponent/WorldTransform.h"
#include "Camera/StaticCamera.h"
#include "Camera/Camera.h"
#include "Rendering/Renderer.h"
#include "Common/MyMath.h"
#include "Scene/Sequence.h"
#include "Number.h"
#include "Scene/ExplosionGame.h"


using namespace DirectX;

Title::Title()
	:mTitleSprite(new Sprite(L"Asset/Title.png"))
	, mHowToPlaySprite(new Sprite(L"Asset/HowToPlay1.jpg"))
	, mCamera(new Camera(DirectX::XMFLOAT3(0.0f, 0.0f, 2.0f), DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),std::make_unique<StaticCamera>()))
	, mBGMSound(AudioSystem::GetInstance().PlayEvent("event:/Title"))
	, mStateHowToPlay(false)
	, mHowToPlayPageNumber(1)
{
	DirectX::XMVECTOR quaternion = Math::QuaternionRotateEuler(0.0f, 0.0f, 0.0f);
	mWorld = std::make_unique<WorldTransform>(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.5f, 1.5f, 1.5f), quaternion);
	mHTPWorld = std::make_unique<WorldTransform>(XMFLOAT3(0.0f, 0.0f, 0.01f), XMFLOAT3(0.8f, 0.8f, 0.8f), quaternion);
}

Title::~Title() {
	mBGMSound.Stop();
}


void Title::ProcessInput() {
	KeyInput::UpdateCurrentKey();
}


NextScene Title::Update() {
	AudioSystem& audioSystem = AudioSystem::GetInstance();
	audioSystem.Update();
	Scene* next = this;
	if (IsPushedStartButton() && !mStateHowToPlay) {
		audioSystem.PlayEvent("event:/Explosion");
		next = new ExplosionGame();
		return NextScene(next,true);
	}
	if (IsPushedHowToPlay()) {
		mStateHowToPlay = true;
	}
	// 遊び方表示中に右クリックしたら、次のページのスプライトにかえる
	if (mStateHowToPlay && KeyInput::GetKeyState(VK_RBUTTON) == EPush) {
		if (mHowToPlayPageNumber == 1) {
			mHowToPlaySprite.reset(new Sprite(L"Asset/HowToPlay2.jpg"));
			mHowToPlayPageNumber = 2;
		}
		else if (mHowToPlayPageNumber == 2) {
			mHowToPlaySprite.reset(new Sprite(L"Asset/HowToPlay3.jpg"));
			mHowToPlayPageNumber = 3;
		}
		else if (mHowToPlayPageNumber == 3) {
			mHowToPlaySprite.reset(new Sprite(L"Asset/HowToPlay1.png"));
			mStateHowToPlay = false;
			mHowToPlayPageNumber = 1;
		}
		
	}

	return NextScene(next, false);
}

void Title::Render() {
	Renderer::BeginRender();
	mTitleSprite->Render(mWorld->GetWorldMat(), *mCamera.get());
	if (mStateHowToPlay) {
		mHowToPlaySprite->Render(mHTPWorld->GetWorldMat(),*mCamera.get());
	}
	Renderer::EndRender();
}


bool Title::IsPushedStartButton() {
	if (!KeyInput::GetKeyState('S') == EPush) return false;
	return true;
}

bool Title::IsPushedHowToPlay() {
	if (!KeyInput::GetKeyState('E') == EPush) return false;
	return true;
}

