#include <stdexcept>
#define WIN32_LEAN_AND_MEAN 
#include <windows.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include "Minigin.h"

#include <thread>

#include "InputManager.h"
#include "SceneManager.h"
#include "Renderer.h"
#include "ResourceManager.h"

SDL_Window* g_window{};

void PrintSDLVersion()
{
	SDL_version version{};
	SDL_VERSION(&version);
	printf("We compiled against SDL version %u.%u.%u ...\n",
		version.major, version.minor, version.patch);

	SDL_GetVersion(&version);
	printf("We are linking against SDL version %u.%u.%u.\n",
		version.major, version.minor, version.patch);

	SDL_IMAGE_VERSION(&version);
	printf("We compiled against SDL_image version %u.%u.%u ...\n",
		version.major, version.minor, version.patch);

	version = *IMG_Linked_Version();
	printf("We are linking against SDL_image version %u.%u.%u.\n",
		version.major, version.minor, version.patch);

	SDL_TTF_VERSION(&version)
	printf("We compiled against SDL_ttf version %u.%u.%u ...\n",
		version.major, version.minor, version.patch);

	version = *TTF_Linked_Version();
	printf("We are linking against SDL_ttf version %u.%u.%u.\n",
		version.major, version.minor, version.patch);
}

svengine::Minigin::Minigin()
{
	PrintSDLVersion();
	
	if (SDL_Init(SDL_INIT_VIDEO) != 0) 
	{
		throw std::runtime_error(std::string("SDL_Init Error: ") + SDL_GetError());
	}

	if (SDL_Init(SDL_INIT_AUDIO) < 0)
	{
		throw std::runtime_error(std::string("SDL_Init Sound Error: ") + SDL_GetError());
	}

	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
	{
		throw std::runtime_error(std::string("Mix_OpenAudio failed: ") + Mix_GetError());
	}

	g_window = SDL_CreateWindow(
		"Programming 4 assignment",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		640,
		480,
		SDL_WINDOW_OPENGL
	);
	if (g_window == nullptr) 
	{
		throw std::runtime_error(std::string("SDL_CreateWindow Error: ") + SDL_GetError());
	}

	Renderer::GetInstance().Init(g_window);

	ResourceManager::GetInstance().Init();
}

svengine::Minigin::~Minigin()
{
	Renderer::GetInstance().Destroy();
	SDL_DestroyWindow(g_window);
	Mix_CloseAudio();
	Mix_Quit();
	SDL_QuitSubSystem(SDL_INIT_AUDIO);
	g_window = nullptr;
	SDL_Quit();
}

void svengine::Minigin::Run(const std::function<void()>& load)
{
	load();

	auto& renderer = Renderer::GetInstance();
	auto& sceneManager = SceneManager::GetInstance();
	auto& input = InputManager::GetInstance();

	sceneManager.Start();

	bool doContinue = true;
	auto previous = std::chrono::high_resolution_clock::now();

	while (doContinue)
	{
		const auto currentTime = std::chrono::high_resolution_clock::now();
		const float elapsedSec = std::chrono::duration<float>(currentTime - previous).count();
		previous = currentTime;

		doContinue = input.ProcessInput();

		// Running an update with elapsedSec for everything besides Physics and AI updates
		sceneManager.Update(elapsedSec);

		// Running the late update with elapsedSec
		sceneManager.LateUpdate(elapsedSec);

		renderer.Render();

		// Preventing additional frame generation by setting a desired frame rate
		const auto sleep_time = currentTime + std::chrono::milliseconds(6) - std::chrono::high_resolution_clock::now();
		std::this_thread::sleep_for(sleep_time);
	}
}
