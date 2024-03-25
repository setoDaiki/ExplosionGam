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
	/// [�@�\] ���������^�C���������擾����
	/// [�߂�l] �����^�C�����ǂ���
	//---------------------------------------------------
	static bool IsRunTime();

	//---------------------------------------------------
	/// [�@�\] ���ݐݒ肳��Ă���V�[�����J�n���܂�
	//---------------------------------------------------
	void RunScene();
	

private:
	class Scene* mScene;
	static bool mIsRunTime;
};
