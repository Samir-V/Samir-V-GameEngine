#pragma once
#include <string>

class SoundSystem
{
public:
	virtual ~SoundSystem() = default;

	/*SoundSystem(const SoundSystem&) = delete;
	SoundSystem(SoundSystem&&) = delete;
	SoundSystem& operator=(const SoundSystem&) = delete;
	SoundSystem& operator=(SoundSystem&&) = delete;*/

	virtual void Play(const std::string& sound, const float volume, bool isMusic = false) = 0;
	virtual void StopMusic() = 0;
	virtual void LoadSound(const std::string& sound, bool isMusic = false) = 0;
	virtual void SetFolder(std::string folderName) = 0;
};