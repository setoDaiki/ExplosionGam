#include"Scene.h"
#include "Sequence.h"
#include "AudioSystem.h"

Scene::Scene() {
	AudioSystem::GetInstance().StopAllSound();
}

Scene::~Scene() {

}
//--------------------------------------------
//   �V�[�����J�n���܂�
//--------------------------------------------
NextScene Scene::Run() {
	ProcessInput();
	NextScene scene = Update();
	Render();
	return scene;
}