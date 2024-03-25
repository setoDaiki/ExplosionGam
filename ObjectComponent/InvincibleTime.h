#pragma once

using AlreadyInvincible = bool;

class InvincibleTime {
public:
	InvincibleTime(unsigned duration);

	//-----------------------------------
	/// [機能] 無敵時間の開始
	/// [返り値] 既に無敵時間だったか
	//-----------------------------------
	AlreadyInvincible Start();
	void Update();
	bool IsInvincible();


private:
	const unsigned mDuration;
	unsigned mElapsedTime;
	bool mIsInvincible;

};