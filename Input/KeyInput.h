#pragma once
#include <windows.h>
#include <XInput.h>
#include <DirectXMath.h>

#pragma comment(lib,"xinput.lib")

//===============================================
//  key�̏�ԗ񋓌^
//===============================================
enum KeyState {
	EPush,
	EHold,
	ERelease,
	ENone
};

class KeyInput {
public:
	

	//-------------------------------------------------------------------
	/// [�@�\] �ŐV��key��State�̎擾���܂�
	/// [����] DirecInput�ɒ�`���ꂽkey�̖��O�ł�(���Ԃ�int�^�ł��邾��)
	/// [�߂�l] �ŐV��key��state�ł�
	//-------------------------------------------------------------------
	static KeyState GetKeyState(int keyCode);
	static KeyState GetButtonState(int button);
	static DirectX::XMFLOAT2 GetStickAxis();

	//-------------------------------------------------------------------------------
	/// [�@�\] �ŐV�̉����ꂽkey��keyState�ƍŐV�̉����ꂽkye���X�V���܂�
	/// [�g�p���@/���ӓ_] �C���X�^���X�̐��֌W�Ȃ����̃��[�v�ɂ���x�����Ăяo���܂�
	//-------------------------------------------------------------------------------
	static void UpdateCurrentKey();

private:
	KeyInput();    // �V���O���g���Ȃ̂ŃR���X�g���N�^�[����
	~KeyInput();   // �V���O���g���Ȃ̂Ńf�X�g���N�^�[����
	KeyInput(const KeyInput&);  // �V���O���g���Ȃ̂ŃR�s�[�R���X�g���N�^�[����

	static void UpdateGamePadState();
	static void UpdateKeyboardKeyState();
	static BYTE mPrevKey[256];
	static BYTE mCurrentKey[256];
	static XINPUT_GAMEPAD mPrevGamePad;
	static XINPUT_GAMEPAD mCurrentGamePad;
	static DirectX::XMFLOAT2 mRStickAxis;
};

