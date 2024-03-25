#include "Sequence.h"
#include "Title.h"
#include <cassert>
#include <iostream>
#include <Windows.h>


bool Sequence::mIsRunTime = false;

Sequence::Sequence() 
	:mScene(new Title())
{
	mIsRunTime = false;
}

Sequence::~Sequence() {
	delete mScene;
}

//--------------------------------------------
//  今がランタイムかどうか
//--------------------------------------------
bool Sequence::IsRunTime() {
	return mIsRunTime;
}


//--------------------------------------------
//   シーンに応じた更新処理を呼び出します
//--------------------------------------------
void Sequence::RunScene() {
	NextScene nextScene = mScene->Run();
	if (nextScene.next != mScene) {
		mIsRunTime = false;
		if (nextScene.haveToDeletePrevScene) {
			delete mScene;
		}
		mScene = nextScene.next;
		mIsRunTime = true;
	}
	
}

