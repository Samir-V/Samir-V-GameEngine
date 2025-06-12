#pragma once
#include <memory>

#include "SoundSystem.h"

class NullSoundSystem final : public SoundSystem
{
	void Play(const std::string&, const float, bool) override {}
	void StopMusic() override {}
	void LoadSound(const std::string&, bool) override {}
	void SetFolder(std::string) override {}
	void SetMute(bool) override {}
};

class ServiceLocator final
{
public:

	static SoundSystem& GetSoundSystem() { return *m_SSInstance; }
	static void RegisterSoundSystem(std::unique_ptr<SoundSystem>&& soundSystem)
	{
		if (soundSystem == nullptr)
		{
			m_SSInstance = std::make_unique<NullSoundSystem>();
		}
		else
		{
			m_SSInstance = std::move(soundSystem);
		}
	}

private:

	static std::unique_ptr<SoundSystem> m_SSInstance;
};

