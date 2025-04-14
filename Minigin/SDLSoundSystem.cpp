#include "SDLSoundSystem.h"

#include <SDL_mixer.h>
#include <unordered_map>
#include <array>
#include <mutex>
#include <thread>
#include <utility>


#include "ResourceManager.h"

class SDLSoundSystem::SDLSoundSystemImpl
{
public:

	SDLSoundSystemImpl();
	~SDLSoundSystemImpl();

	void Play(const std::string& sound, const float volume, bool isMusic = false);
	void Update(std::stop_token stopToken);
	void LoadSound(const std::string& sound, bool isMusic = false);

private:
	struct PlayMessage
	{
		std::string name;
		float volume;
		bool isMusic;
	};

	static constexpr int m_MaxPending{ 16 };
	int	m_NumPending { 0 };

	int m_Head		 { 0 };
	int m_Tail		 { 0 };

	std::jthread			m_SoundUpdateThread{ };

	std::mutex				m_Mtx { };
	std::condition_variable m_CV  { };

	std::array<PlayMessage, m_MaxPending> m_Pending{};

	std::unordered_map<std::string, Mix_Chunk*> m_SoundChunks{};
	std::unordered_map<std::string, Mix_Music*> m_Music{};
};

SDLSoundSystem::SDLSoundSystemImpl::SDLSoundSystemImpl()
{
	m_SoundUpdateThread = std::jthread([this](std::stop_token stopToken)
	{
			Update(std::move(stopToken));
	});
}

SDLSoundSystem::SDLSoundSystemImpl::~SDLSoundSystemImpl()
{
	m_CV.notify_one();
	m_SoundUpdateThread.request_stop();

	for (auto& soundPair : m_SoundChunks)
	{
		if (soundPair.second)
		{
			Mix_FreeChunk(soundPair.second);
		}
	}

	for (auto& soundPair : m_Music)
	{
		if (soundPair.second)
		{
			Mix_FreeMusic(soundPair.second);
		}
	}
}


void SDLSoundSystem::SDLSoundSystemImpl::Play(const std::string& sound, const float volume, bool isMusic)
{
	for (int index = m_Head; index != m_Tail; index = (index + 1) % m_MaxPending)
	{
		if (m_Pending[index].name == sound)
		{
			m_Pending[index].volume = std::max(volume, m_Pending[index].volume);
			return;
		}
	}

	assert((m_Tail + 1) % m_MaxPending != m_Head);

	m_Pending[m_Tail].name = sound;
	m_Pending[m_Tail].volume = volume;
	m_Pending[m_Tail].isMusic = isMusic;

	m_Tail = (m_Tail + 1) % m_MaxPending;
	m_NumPending++;
	m_CV.notify_one();
}

void SDLSoundSystem::SDLSoundSystemImpl::Update(std::stop_token stopToken)
{
	while (!stopToken.stop_requested())
	{
		std::unique_lock<std::mutex> lck(m_Mtx);
		m_CV.wait(lck, [this, &stopToken] {return m_NumPending != 0 || stopToken.stop_requested(); });

		if (m_Head == m_Tail)
		{
			return;
		}

		const auto& playMessage = m_Pending[m_Head];

		if (playMessage.isMusic)
		{
			auto it = m_Music.find(playMessage.name);

			if (it == m_Music.end())
			{
				LoadSound(playMessage.name, true);
			}

			Mix_Music* music = m_Music.at(playMessage.name);

			if (music)
			{
				Mix_VolumeMusic(static_cast<int>(playMessage.volume * MIX_MAX_VOLUME));
				Mix_PlayMusic(music, -1);
			}
		}
		else
		{
			auto it = m_SoundChunks.find(playMessage.name);

			if (it == m_SoundChunks.end())
			{
				LoadSound(playMessage.name);
			}

			Mix_Chunk* soundChunk = m_SoundChunks.at(playMessage.name);

			if (soundChunk)
			{
				Mix_VolumeChunk(soundChunk, static_cast<int>(playMessage.volume * MIX_MAX_VOLUME));
				Mix_PlayChannel(-1, soundChunk, 0);
			}
		}
		m_Head = (m_Head + 1) % m_MaxPending;
		m_NumPending--;
	}
}

void SDLSoundSystem::SDLSoundSystemImpl::LoadSound(const std::string& sound, bool isMusic)
{
	if (isMusic)
	{
		m_Music.insert({ sound, Mix_LoadMUS(sound.c_str()) });
	}
	else
	{
		m_SoundChunks.insert({ sound, Mix_LoadWAV(sound.c_str()) });
	}
}

SDLSoundSystem::SDLSoundSystem()
{
	m_pImpl = std::make_unique<SDLSoundSystemImpl>();
}

SDLSoundSystem::~SDLSoundSystem() = default;


void SDLSoundSystem::Play(const std::string& sound, const float volume, bool isMusic)
{
	m_pImpl->Play(sound, volume, isMusic);
}

void SDLSoundSystem::LoadSound(const std::string& sound, bool isMusic)
{
	m_pImpl->LoadSound(sound, isMusic);
}


