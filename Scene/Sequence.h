#pragma once
#include <memory>

struct NextScene {
	class Scene* next;
	bool haveToDeletePrevScene;
};

class Sequence {
public:
	Sequence();
	~Sequence();


	//---------------------------------------------------
	/// [機能] 今がランタイム中かを取得する
	/// [戻り値] ランタイムかどうか
	//---------------------------------------------------
	static bool IsRunTime();

	//---------------------------------------------------
	/// [機能] 現在設定されているシーンを開始します
	//---------------------------------------------------
	void RunScene();
	

private:
	class Scene* mScene;
	static bool mIsRunTime;
};
