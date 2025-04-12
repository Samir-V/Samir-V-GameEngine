#pragma once
#include <memory>

#include "SoundSystem.h"

class ServiceLocator final
{
public:

	static SoundSystem& GetSoundSystem() { return *m_SSInstance; }
	static void RegisterSoundSystem(std::unique_ptr<SoundSystem>&& soundSystem) { m_SSInstance = std::move(soundSystem); }

private:

	static std::unique_ptr<SoundSystem> m_SSInstance;
};

