#include"Scene.h"
#include "Sequence.h"
#include "AudioSystem.h"

Scene::Scene() {
	AudioSystem::GetInstance().StopAllSound();
}

Scene::~Scene() {

}
//--------------------------------------------
//   シーンを開始します
//--------------------------------------------
NextScene Scene::Run() {
	ProcessInput();
	NextScene scene = Update();
	Render();
	return scene;
}