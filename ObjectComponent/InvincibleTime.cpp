#include "InvincibleTime.h"

InvincibleTime::InvincibleTime(unsigned duration) 
	:mDuration(duration)
	,mElapsedTime(0)
	,mIsInvincible(false)
{

}


AlreadyInvincible InvincibleTime::Start() {
	if (mIsInvincible) return true;
	mIsInvincible = true;
	return false;
}


void InvincibleTime::Update() {
	if (!mIsInvincible) return;
	mElapsedTime += 1;
	if (mElapsedTime >= mDuration) {
		mIsInvincible = false;
		mElapsedTime = 0;
	}

}

bool InvincibleTime::IsInvincible() {
	return mIsInvincible;
}