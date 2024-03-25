#pragma once
#include <unordered_map>
#include <string>
#include <memory>
#include <DirectXMath.h>
#pragma comment(lib,"fmodL64_vc.lib")
#pragma comment(lib,"fmodstudioL64_vc.lib")

namespace FMOD
{
	class System;
	namespace Studio
	{
		class Bank;
		class EventDescription;
		class EventInstance;
		class System;
		class Bus;
	};
};



class AudioSystem {
public:
	class SoundEvent {
	public:
		SoundEvent(FMOD::Studio::EventInstance& event);

		bool Is3D()const;
		void Set3DAttributes(const DirectX::XMFLOAT3& audioPos);
		void Restart();
		void Stop();
		void SetPaused();
		void SetVolume(float volume);
		void SetPitch(float pitch);
		void SetParameter(const std::string& name, float parameter);
		bool GetPaused() const;
		float GetVolume() const;
		float GetPitch() const;
		float GetParameter(const std::string& name)const;

	private:
		FMOD::Studio::EventInstance& mEvent;
	};

	static AudioSystem& GetInstance();
	void Destory();
	void LoadBank(const std::string& path);
	void UnloadBank(const std::string& name);
	void UnloadAllBanks();
	void SetListenerPos(const DirectX::XMFLOAT3& listenerPos);
	void Update();
	void StopAllSound();
	AudioSystem::SoundEvent PlayEvent(const std::string& name);

private:
	void Init();

	static AudioSystem mInstance;

	std::unordered_map<std::string, FMOD::Studio::Bank*> mBanks;
	std::unordered_map<std::string, FMOD::Studio::EventDescription*> mEvents;
	std::unordered_map<unsigned, FMOD::Studio::EventInstance*> mEventInstances;
	std::unordered_map<std::string, FMOD::Studio::Bus*> mBuses;
	FMOD::Studio::System* mSystem;
	FMOD::System* mLowLevelSystem;
	int mNextID;
	AudioSystem():mSystem(nullptr),mNextID(0),mLowLevelSystem(nullptr) {};
	AudioSystem(AudioSystem&) = delete;
	~AudioSystem();

};
