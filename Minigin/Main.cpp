#include <SDL.h>

#if _DEBUG
// ReSharper disable once CppUnusedIncludeDirective
#if __has_include(<vld.h>)
#include <vld.h>
#endif
#endif

#include "CacheTrasherComponent.h"
#include "FPSComponent.h"
#include "Minigin.h"
#include "SceneManager.h"
#include "ResourceManager.h"
#include "TextComponent.h"
#include "GameObject.h"
#include "InputManager.h"
#include "PeterPepperCommand.h"
#include "PeterPepperComponent.h"
#include "Scene.h"
#include "Texture2DComponent.h"

void load()
{
	auto& scene = dae::SceneManager::GetInstance().CreateScene("MainScene");
	auto& input = dae::InputManager::GetInstance();

	auto go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("background.tga");
	scene.Add(std::move(go));	

	go = std::make_unique<dae::GameObject>();
	auto textureComp = go->AddComponent<dae::Texture2DComponent>("logo.tga");
	textureComp->SetLocalPosition(216, 180);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	auto font = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 28);
	auto textComp = go->AddComponent<dae::TextComponent>("Programming 4 Assignment", font);
	textComp->SetLocalPosition(140, 40);
	scene.Add(std::move(go));

	// FPS display
	go = std::make_unique<dae::GameObject>();
	textComp = go->AddComponent<dae::TextComponent>("60.0 FPS", font);
	textComp->SetLocalPosition(10, 10);
	go->AddComponent<dae::FPSComponent>(textComp);
	scene.Add(std::move(go));

	// Keyboard Peter Pepper
	go = std::make_unique<dae::GameObject>();
	go->SetLocalPosition(300.0f, 300.0f);
	go->AddComponent<dae::Texture2DComponent>("PeterPepper.png");
	go->AddComponent<dae::PeterPepperComponent>(350.0f);

	input.RegisterCommand(std::make_unique<MoveUp>(go.get()), SDL_SCANCODE_W, dae::InputManager::ActivationType::Holding);
	input.RegisterCommand(std::make_unique<MoveLeft>(go.get()), SDL_SCANCODE_A, dae::InputManager::ActivationType::Holding);
	input.RegisterCommand(std::make_unique<MoveDown>(go.get()), SDL_SCANCODE_S, dae::InputManager::ActivationType::Holding);
	input.RegisterCommand(std::make_unique<MoveRight>(go.get()), SDL_SCANCODE_D, dae::InputManager::ActivationType::Holding);

	scene.Add(std::move(go));
}

int main(int, char*[]) {
	dae::Minigin engine("../Data/");
	engine.Run(load);
    return 0;
}