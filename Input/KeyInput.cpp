#include "KeyInput.h"
#include "App.h"
#include <cmath>
#include <iostream>
#include <string.h>
#include <cassert>


BYTE KeyInput::mCurrentKey[256];
BYTE KeyInput::mPrevKey[256];
XINPUT_GAMEPAD KeyInput::mPrevGamePad;
XINPUT_GAMEPAD KeyInput::mCurrentGamePad;
DirectX::XMFLOAT2 KeyInput::mRStickAxis;

KeyInput::KeyInput() {
	
}

KeyInput::~KeyInput() {

}

//-------------------------------------------
//  KeyStateの取得
//-------------------------------------------
KeyState KeyInput::GetKeyState(int keyCode) {
	// keyStateの更新

	// 前のフレームで押されていたら
	if (mPrevKey[keyCode] & 0x80) {
		// 今回も押されているなら
		if (mCurrentKey[keyCode] & 0x80) {
			return EHold; // Hold状態を返す
		}
		//今回は押されていないなら
		else {
			return ERelease; // Release状態を返す
		}
	}

	//前のフレームで押されていないなら
	else {
		//今回は押されているなら
		if (mCurrentKey[keyCode] & 0x80) {
			return EPush; // Push状態を返す
		}
		//今回も押されていないなら
		else {
			return ENone; // None状態を返す
		}
	}
}

//------------------------------
//  GamePadボタン状態の取得
//------------------------------
KeyState KeyInput::GetButtonState(int button) {
	// 前のフレームで押されていたら
	if (mPrevGamePad.wButtons & button) {
		// 今回も押されているなら
		if (mCurrentGamePad.wButtons & button) {
			return EHold; // Hold状態を返す
		}
		//今回は押されていないなら
		else {
			return ERelease; // Release状態を返す
		}
	}

	//前のフレームで押されていないなら
	else {
		//今回は押されているなら
		if (mCurrentGamePad.wButtons & button) {
			return EPush; // Push状態を返す
		}
		//今回も押されていないなら
		else {
			return ENone; // None状態を返す
		}
	}
}

//---------------------------
//  スティック座標の取得
//---------------------------
DirectX::XMFLOAT2 KeyInput::GetStickAxis() {
	return mRStickAxis;
}


//--------------------------------------------
//　　最新のkeyと最新のkeyStateの更新
//--------------------------------------------
void KeyInput::UpdateCurrentKey() {
	UpdateGamePadState();
	UpdateKeyboardKeyState();
}


//----------------------------------------
//  GamePad状態の更新
//----------------------------------------
void KeyInput::UpdateGamePadState() {
	XINPUT_STATE s;
	ZeroMemory(&s, sizeof(s));
	if (XInputGetState(0, &s) != ERROR_SUCCESS) return;

	// デッドゾーンのコントローラー入力は0に
	if (s.Gamepad.sThumbLX <  XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
		s.Gamepad.sThumbLX > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) {
		s.Gamepad.sThumbLX = 0;
	}
	if (s.Gamepad.sThumbLY <  XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
		s.Gamepad.sThumbLY > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
	{
		s.Gamepad.sThumbLY = 0;
	}
	mPrevGamePad = mCurrentGamePad;
	mCurrentGamePad = s.Gamepad;

	// 右スティック入力値を独自の方法で保存(値を-1 ～ 1にする)
	float x = static_cast<float>(s.Gamepad.sThumbLX);
	float y = static_cast<float>(s.Gamepad.sThumbLY);
	if (x < 0.0f) {
		x = x / 32768.0f;
	}
	else {
		x = x / 32767.0f;
	}
	if (y < 0.0f) {
		y = y / 32768.0f;
	}
	else {
		y = y / 32767.0f;
	}
	mRStickAxis.x = x;
	mRStickAxis.y = y;
	
}

//---------------------------------------
//  キーボード状態の更新
//---------------------------------------
void KeyInput::UpdateKeyboardKeyState() {
	memcpy(mPrevKey, mCurrentKey, sizeof(BYTE) * 256);
	// 最新のkeyの更新
	if (!GetKeyboardState(mCurrentKey)) {
		assert(!"最新のキーボードステートの取得に失敗");
	}
}

