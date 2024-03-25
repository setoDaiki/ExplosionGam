#include "AudioSystem.h"
#include <windows.h>
#include <fmod_studio.hpp>
#include <fmod_errors.h>
#include <vector>
#include <cassert>
#include <string>
#ifdef _DEBUG
#include <iostream>
#endif

AudioSystem AudioSystem::mInstance;


void AudioSystem::Init() {
	// FMODデバックの初期化
	FMOD::Debug_Initialize(
		FMOD_DEBUG_LEVEL_ERROR,
		FMOD_DEBUG_MODE_TTY
	);
	FMOD_RESULT result = FMOD::Studio::System::create(&mSystem);
	if (result != FMOD_OK) {
		assert(!FMOD_ErrorString(result));
	}
	const unsigned numMaxConcurrentSounds = 512;
	result = mSystem->initialize(
		numMaxConcurrentSounds,
		FMOD_STUDIO_INIT_NORMAL,
		FMOD_INIT_NORMAL,
		nullptr
	);
	if (result != FMOD_OK) {
		assert(FMOD_ErrorString(result));
	}

	mSystem->getLowLevelSystem(&mLowLevelSystem);

	LoadBank("FmodStudio/ExplosionGame/Build/Desktop/Master Bank.strings.bank");
	LoadBank("FmodStudio/ExplosionGame/Build/Desktop/Master Bank.bank");
}

AudioSystem::~AudioSystem() {
	UnloadAllBanks();
	if (mSystem) {
		mSystem->release();
	}
}


void AudioSystem::Destory() {
	
}


AudioSystem& AudioSystem::GetInstance() {
	static bool isFirst = true;
	if (isFirst) {
		mInstance.Init();
		isFirst = false;
	}
	return mInstance;
}

void AudioSystem::LoadBank(const std::string& path) {
	if (mBanks.find(path) != mBanks.end()) {
		return;
	}
	FMOD::Studio::Bank* bank = nullptr;  // ここにバンクをロードする
	FMOD_RESULT result = mSystem->loadBankFile(
		path.c_str(),
		FMOD_STUDIO_LOAD_BANK_NORMAL,
		&bank
	);
	assert(result == FMOD_OK);
	const unsigned maxPathLength = 512;
	mBanks.emplace(path,bank); // ロードしたバンクをハッシュに追加
	bank->loadSampleData();    // オーディオデータをバンクにロード
	int numEvents = 0;
	bank->getEventCount(&numEvents);
	if (numEvents > 0) {   // イベントがあるなら
		std::vector<FMOD::Studio::EventDescription*> events(numEvents);
		bank->getEventList(events.data(),numEvents,&numEvents);
		char eventName[maxPathLength];
		// mEventsにeventとイベント名を追加していく
		for (int i = 0; i < numEvents;i++) {
			FMOD::Studio::EventDescription* event = events[i];
			event->getPath(eventName,maxPathLength,nullptr);
#ifdef _DEBUG
			//std::cout << eventName << '\n';
#endif
			mEvents.emplace(eventName,event);
		}

	}
	int numBuses = 0;
	bank->getBusCount(&numBuses);
	if (numBuses > 0) {  // バスがあるなら
		std::vector<FMOD::Studio::Bus*> buses(numBuses);
		bank->getBusList(buses.data(),numBuses,&numBuses);
		char busName[maxPathLength];
		// mBusesにbusとバス名を追加していく
		for (int i = 0; i < numBuses;i++) {
			FMOD::Studio::Bus* bus = buses[i];
			bus->getPath(busName,maxPathLength,nullptr);
			mBuses.emplace(busName,bus);
		}
	}


}




void AudioSystem::UnloadBank(const std::string& name) {
	auto iter = mBanks.find(name);
	if (iter == mBanks.end()) {
		return;
	}
	FMOD::Studio::Bank* bank = iter->second;
	int numEvents = 0;
	bank->getEventCount(&numEvents);
	const unsigned maxPathLength = 512;
	if (numEvents > 0) {
		std::vector<FMOD::Studio::EventDescription*> events(numEvents);
		bank->getEventList(events.data(),numEvents,&numEvents);
		char eventName[maxPathLength];
		for (int i = 0; i < numEvents;i++) {
			FMOD::Studio::EventDescription* event = events[i];
			event->getPath(eventName,maxPathLength,nullptr);
			auto unloadEvent = mEvents.find(eventName);
			if (unloadEvent != mEvents.end()) {
				mEvents.erase(unloadEvent);
			}
		}
	}
	int numBuses = 0;
	bank->getBusCount(&numBuses);
	if (numBuses > 0) {
		std::vector<FMOD::Studio::Bus*> buses(numBuses);
		bank->getBusList(buses.data(), numBuses, &numBuses);
		char busName[maxPathLength];
		for (int i = 0; i < numBuses; i++) {
			FMOD::Studio::Bus* bus = buses[i];
			bus->getPath(busName, maxPathLength, nullptr);
			auto unloadBus = mBuses.find(busName);
			if (unloadBus != mBuses.end()) {
				mBuses.erase(unloadBus);
			}
		}

	}
	bank->unloadSampleData();
	bank->unload();
	mBanks.erase(iter);  // これメモリリークしないの?
}


void AudioSystem::UnloadAllBanks() {
	for (auto& iter : mBanks) {
		iter.second->unloadSampleData();
		iter.second->unload();
	}
	mBanks.clear();
	mEvents.clear();
}

void AudioSystem::Update() {
	std::vector<unsigned> doneEventIds;
	for (auto& iter: mEventInstances) {
		FMOD::Studio::EventInstance* eventInstance = iter.second;
		FMOD_STUDIO_PLAYBACK_STATE instanceState;
		eventInstance->getPlaybackState(&instanceState);
		// 用済みのサウンドは削除しちゃう
		if (instanceState == FMOD_STUDIO_PLAYBACK_STOPPED) {
			eventInstance->release();
			doneEventIds.emplace_back(iter.first);  // 消してる
		}
	}

	for (auto id : doneEventIds) {
		mEventInstances.erase(id);
	}
	mSystem->update();
}

void AudioSystem::StopAllSound() {
	for (auto& iter : mEventInstances) {
		FMOD_STUDIO_STOP_MODE mode = FMOD_STUDIO_STOP_ALLOWFADEOUT;
		iter.second->stop(mode);
	}

}


namespace {
	FMOD_VECTOR ToFmodVector(const DirectX::XMFLOAT3& vector) {
		FMOD_VECTOR fVector;
		fVector.x = vector.x;
		fVector.y = vector.y;
		fVector.z = vector.z;
		return fVector;
	}
}


void AudioSystem::SetListenerPos(const DirectX::XMFLOAT3& listenerPos) {
	/*--------------------------------------------
	   [TODO]
	   fowardとupは仮。あとで直した方がいい
	----------------------------------------------*/
	FMOD_3D_ATTRIBUTES listener;
	listener.position = ToFmodVector(listenerPos);
	listener.forward = { 0.0f,0.0f,1.0f };
	listener.up = { 0.0f,1.0f,0.0f };
	listener.velocity = { 0.0f,0.0f,0.0f };
	mSystem->setListenerAttributes(0,&listener);
}


AudioSystem::SoundEvent AudioSystem::PlayEvent(const std::string& name) {
	auto iter = mEvents.find(name);
	FMOD::Studio::EventInstance* eventInstance = nullptr;
	assert(iter != mEvents.end());
	if (iter != mEvents.end()) {
		iter->second->createInstance(&eventInstance);
		if (eventInstance) {
			eventInstance->start();
			mNextID++;
			mEventInstances.emplace(mNextID, eventInstance);
		}
	}
	return AudioSystem::SoundEvent(*eventInstance);
}






//------------------------------    ここからは SoundEvent  クラス    --------------------------------


AudioSystem::SoundEvent::SoundEvent(FMOD::Studio::EventInstance & event)
	:mEvent(event)
{

}


bool AudioSystem::SoundEvent::Is3D()const {
	bool is3D = false;
	if (&mEvent) {
		FMOD::Studio::EventDescription* ed = nullptr;
		mEvent.getDescription(&ed);
		if (ed) {
			ed->is3D(&is3D);
		}
	}
	return is3D;
}

void AudioSystem::SoundEvent::Set3DAttributes(const DirectX::XMFLOAT3& audioPos) {
	if (&mEvent) {
		FMOD_3D_ATTRIBUTES audio3D;
		audio3D.position = ToFmodVector(audioPos);
		audio3D.forward = { 0.0f,0.0f,1.0f };
		audio3D.up = { 0.0f,1.0f,0.0f };
		audio3D.velocity = { 0.0f,0.0f,0.0f };
		mEvent.set3DAttributes(&audio3D);
	}
}


void AudioSystem::SoundEvent::Restart() {
	if (&mEvent) {
		mEvent.start();
	}
}


void AudioSystem::SoundEvent::Stop() {
	if (&mEvent) {
		FMOD_STUDIO_STOP_MODE mode = FMOD_STUDIO_STOP_ALLOWFADEOUT;
		mEvent.stop(mode);
	}
}


void AudioSystem::SoundEvent::SetPaused() {
	if (&mEvent) {
		mEvent.setPaused(true);
	}
}


void AudioSystem::SoundEvent::SetVolume(float volume) {
	if (&mEvent) {
		mEvent.setVolume(volume);
	}
}


void AudioSystem::SoundEvent::SetPitch(float pitch) {
	if (&mEvent) {
		mEvent.setPitch(pitch);
	}
}


void AudioSystem::SoundEvent::SetParameter(const std::string& name,float parameter) {
	if (&mEvent) {
		mEvent.setParameterValue(name.c_str(),parameter);
	}
}


bool AudioSystem::SoundEvent::GetPaused()const {
	bool isPaused = true;
	if (&mEvent) {
		mEvent.getPaused(&isPaused);
	}
	return isPaused;
}


float AudioSystem::SoundEvent::GetVolume()const {
	float volume = 1.0f;
	if (&mEvent) {
		mEvent.getVolume(&volume);
	}
	return volume;
}


float AudioSystem::SoundEvent::GetPitch()const {
	float pitch = 1.0f;
	if (&mEvent) {
		mEvent.getPitch(&pitch);
	}
	return pitch;
}


float AudioSystem::SoundEvent::GetParameter(const std::string& name)const {
	float parameter = 0.0f;
	if (&mEvent) {
		mEvent.getParameterValue(name.c_str(), &parameter);
	}
	return parameter;
}







