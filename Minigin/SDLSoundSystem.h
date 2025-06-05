#pragma once
#include <memory>

#include "SoundSystem.h"

class SDLSoundSystem final : public SoundSystem
{
public:
	SDLSoundSystem();
	~SDLSoundSystem() override;

	void Play(const std::string& sound, const float volume, bool isMusic = false) override;
	void StopMusic() override;
	void LoadSound(const std::string& sound, bool isMusic = false) override;
	void SetFolder(std::string folderName) override;

private:

	class SDLSoundSystemImpl;

	std::unique_ptr<SDLSoundSystemImpl> m_pImpl;
};

