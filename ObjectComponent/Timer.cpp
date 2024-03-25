#include "Timer.h"
#include "Number.h"
#include "Camera/Camera.h"
#include <iostream>

Timer::Timer(const DirectX::XMFLOAT3& pos) 
	:mStartTime(timeGetTime())
	,mNumberUI(new NumberUI(pos,DirectX::XMFLOAT3(0.1f,0.1f,0.1f)))
	,mCumulativeStopTime(0)
	,mStopStartTime(0)
{

}

Timer::~Timer() {

}


DWORD Timer::GetTime() {
	if (mIsStop) {
		return (mStopStartTime - mStartTime - mCumulativeStopTime) /1000;
	}
	return (timeGetTime() - mStartTime - mCumulativeStopTime) / 1000;
}

void Timer::Stop() {
	if (mIsStop) return;
	mStopStartTime = timeGetTime();;
	mIsStop = true;
}

void Timer::ReleaseStop() {
	if (!mIsStop) return;
	mCumulativeStopTime += ((timeGetTime() - mStopStartTime) / 1000);
	mStopStartTime = 0;
	mIsStop = false;
}


void Timer::Render(const class Camera& camera) {
	mNumberUI->Render(camera, GetTime());
}