#pragma once
#include <winuser.h>
#include <windef.h>
#include <iostream>
#include "Common/MyMath.h"

class Mouse {
public:
	Mouse() {}
	~Mouse() {}

	static void Update() {
		mPrevCursorPos = mCurrentCursorPos;
		GetCursorPos(&mCurrentCursorPos);
		const bool cursorInXEdge = (mCurrentCursorPos.x >= 2500 || mCurrentCursorPos.x <= 100);
		const bool cursorInYEdge = (mCurrentCursorPos.y >= 1300 || mCurrentCursorPos.y <= 100);
		if (cursorInXEdge || cursorInYEdge) {
			ResetMousePos();
			mCurrentCursorPos = POINT(1280, 720);
			mPrevCursorPos = POINT(1280,720);
		}
		
	}

	static POINT GetRelativeMovement() {
		POINT retPoint = POINT(mCurrentCursorPos.x - mPrevCursorPos.x,mCurrentCursorPos.y - mPrevCursorPos.y);
		return retPoint;
	}

	static void ResetMousePos() {
		mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE, 32768, 32768, 0, NULL);
	}

	

private:
	static POINT mCurrentCursorPos;
	static POINT mPrevCursorPos;

};

POINT Mouse::mCurrentCursorPos(1280,720);
POINT Mouse::mPrevCursorPos(1280,720);
