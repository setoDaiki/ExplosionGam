#pragma once

using AlreadyInvincible = bool;

class InvincibleTime {
public:
	InvincibleTime(unsigned duration);

	//-----------------------------------
	/// [�@�\] ���G���Ԃ̊J�n
	/// [�Ԃ�l] ���ɖ��G���Ԃ�������
	//-----------------------------------
	AlreadyInvincible Start();
	void Update();
	bool IsInvincible();


private:
	const unsigned mDuration;
	unsigned mElapsedTime;
	bool mIsInvincible;

};