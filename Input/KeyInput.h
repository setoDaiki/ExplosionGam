#pragma once
#include <windows.h>
#include <XInput.h>
#include <DirectXMath.h>

#pragma comment(lib,"xinput.lib")

//===============================================
//  keyの状態列挙型
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
	/// [機能] 最新のkeyのStateの取得します
	/// [引数] DirecInputに定義されたkeyの名前です(実態がint型であるだけ)
	/// [戻り値] 最新のkeyのstateです
	//-------------------------------------------------------------------
	static KeyState GetKeyState(int keyCode);
	static KeyState GetButtonState(int button);
	static DirectX::XMFLOAT2 GetStickAxis();

	//-------------------------------------------------------------------------------
	/// [機能] 最新の押されたkeyのkeyStateと最新の押されたkyeを更新します
	/// [使用方法/注意点] インスタンスの数関係なく一回のループにつき一度だけ呼び出します
	//-------------------------------------------------------------------------------
	static void UpdateCurrentKey();

private:
	KeyInput();    // シングルトンなのでコンストラクター封印
	~KeyInput();   // シングルトンなのでデストラクター封印
	KeyInput(const KeyInput&);  // シングルトンなのでコピーコンストラクター封印

	static void UpdateGamePadState();
	static void UpdateKeyboardKeyState();
	static BYTE mPrevKey[256];
	static BYTE mCurrentKey[256];
	static XINPUT_GAMEPAD mPrevGamePad;
	static XINPUT_GAMEPAD mCurrentGamePad;
	static DirectX::XMFLOAT2 mRStickAxis;
};

