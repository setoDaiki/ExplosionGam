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
//  KeyState�̎擾
//-------------------------------------------
KeyState KeyInput::GetKeyState(int keyCode) {
	// keyState�̍X�V

	// �O�̃t���[���ŉ�����Ă�����
	if (mPrevKey[keyCode] & 0x80) {
		// �����������Ă���Ȃ�
		if (mCurrentKey[keyCode] & 0x80) {
			return EHold; // Hold��Ԃ�Ԃ�
		}
		//����͉�����Ă��Ȃ��Ȃ�
		else {
			return ERelease; // Release��Ԃ�Ԃ�
		}
	}

	//�O�̃t���[���ŉ�����Ă��Ȃ��Ȃ�
	else {
		//����͉�����Ă���Ȃ�
		if (mCurrentKey[keyCode] & 0x80) {
			return EPush; // Push��Ԃ�Ԃ�
		}
		//�����������Ă��Ȃ��Ȃ�
		else {
			return ENone; // None��Ԃ�Ԃ�
		}
	}
}

//------------------------------
//  GamePad�{�^����Ԃ̎擾
//------------------------------
KeyState KeyInput::GetButtonState(int button) {
	// �O�̃t���[���ŉ�����Ă�����
	if (mPrevGamePad.wButtons & button) {
		// �����������Ă���Ȃ�
		if (mCurrentGamePad.wButtons & button) {
			return EHold; // Hold��Ԃ�Ԃ�
		}
		//����͉�����Ă��Ȃ��Ȃ�
		else {
			return ERelease; // Release��Ԃ�Ԃ�
		}
	}

	//�O�̃t���[���ŉ�����Ă��Ȃ��Ȃ�
	else {
		//����͉�����Ă���Ȃ�
		if (mCurrentGamePad.wButtons & button) {
			return EPush; // Push��Ԃ�Ԃ�
		}
		//�����������Ă��Ȃ��Ȃ�
		else {
			return ENone; // None��Ԃ�Ԃ�
		}
	}
}

//---------------------------
//  �X�e�B�b�N���W�̎擾
//---------------------------
DirectX::XMFLOAT2 KeyInput::GetStickAxis() {
	return mRStickAxis;
}


//--------------------------------------------
//�@�@�ŐV��key�ƍŐV��keyState�̍X�V
//--------------------------------------------
void KeyInput::UpdateCurrentKey() {
	UpdateGamePadState();
	UpdateKeyboardKeyState();
}


//----------------------------------------
//  GamePad��Ԃ̍X�V
//----------------------------------------
void KeyInput::UpdateGamePadState() {
	XINPUT_STATE s;
	ZeroMemory(&s, sizeof(s));
	if (XInputGetState(0, &s) != ERROR_SUCCESS) return;

	// �f�b�h�]�[���̃R���g���[���[���͂�0��
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

	// �E�X�e�B�b�N���͒l��Ǝ��̕��@�ŕۑ�(�l��-1 �` 1�ɂ���)
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
//  �L�[�{�[�h��Ԃ̍X�V
//---------------------------------------
void KeyInput::UpdateKeyboardKeyState() {
	memcpy(mPrevKey, mCurrentKey, sizeof(BYTE) * 256);
	// �ŐV��key�̍X�V
	if (!GetKeyboardState(mCurrentKey)) {
		assert(!"�ŐV�̃L�[�{�[�h�X�e�[�g�̎擾�Ɏ��s");
	}
}

