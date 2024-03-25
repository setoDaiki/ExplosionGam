#pragma once
#include <memory>
#include <windows.h>
#include <DirectXMath.h>

class Timer {
public:
	explicit Timer(const DirectX::XMFLOAT3& pos);
	~Timer();

	void Render(const class Camera& camera);
	DWORD GetTime();
	void Stop();
	void ReleaseStop();

private:
	std::unique_ptr<class NumberUI> mNumberUI;
	DWORD mStartTime;
	unsigned mCumulativeStopTime;
	unsigned mStopStartTime;
	bool mIsStop;
};
