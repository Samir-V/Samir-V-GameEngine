#pragma once
#include <string>

class SoundSystem
{
public:
	virtual ~SoundSystem() = default;
	virtual void Play(const std::string& sound, const float volume, bool isMusic = false) = 0;
	virtual void LoadSound(const std::string& sound, bool isMusic = false) = 0;
};