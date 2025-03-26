#include <SDL.h>

#if _DEBUG
// ReSharper disable once CppUnusedIncludeDirective
#if __has_include(<vld.h>)
#include <vld.h>
#endif
#endif

#include "Minigin.h"

#include "FPSComponent.h"
#include "SceneManager.h"
#include "ResourceManager.h"
#include "TextComponent.h"
#include "GameObject.h"
#include "HealthDisplayComponent.h"
#include "InputManager.h"
#include "PeterPepperCommand.h"
#include "PeterPepperComponent.h"
#include "Scene.h"
#include "ScoreDisplayComponent.h"
#include "Texture2DComponent.h"
#include "Windows.h"
#include "Xinput.h"

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

	// Health Display

	auto healthDisplayGO = std::make_unique<dae::GameObject>();

	textComp = healthDisplayGO->AddComponent<dae::TextComponent>("Remaining Health: 3", font);
	auto healthDisplayComp = healthDisplayGO->AddComponent<dae::HealthDisplayComponent>(textComp);
	healthDisplayGO->SetLocalPosition(20.0f, 130.0f);

	scene.Add(std::move(healthDisplayGO));

	// Score Display

	auto scoreDisplayGO = std::make_unique<dae::GameObject>();

	textComp = scoreDisplayGO->AddComponent<dae::TextComponent>("Score: 0", font);
	auto scoreDisplayComponent = scoreDisplayGO->AddComponent<dae::ScoreDisplayComponent>(textComp);
	scoreDisplayGO->SetLocalPosition(20.0f, 100.0f);

	scene.Add(std::move(scoreDisplayGO));

	// Keyboard Peter Pepper
	go = std::make_unique<dae::GameObject>();
	go->SetLocalPosition(200.0f, 300.0f);
	go->AddComponent<dae::Texture2DComponent>("PeterPepper.png");
	auto peterPepperComp = go->AddComponent<dae::PeterPepperComponent>(150.0f);

	// Subscribing to event
	peterPepperComp->GetObjectDeathEvent()->AddObserver(healthDisplayComp);
	peterPepperComp->GetScoreChangedEvent()->AddObserver(scoreDisplayComponent);

	input.RegisterKeyboardCommand(std::make_unique<MoveInDirection>(go.get(), glm::vec2(0.0f, -1.0f)), SDL_SCANCODE_W, dae::InputManager::ActivationType::Holding);
	input.RegisterKeyboardCommand(std::make_unique<MoveInDirection>(go.get(), glm::vec2(-1.0f, 0.0f)), SDL_SCANCODE_A, dae::InputManager::ActivationType::Holding);
	input.RegisterKeyboardCommand(std::make_unique<MoveInDirection>(go.get(), glm::vec2(0.0f, 1.0f)), SDL_SCANCODE_S, dae::InputManager::ActivationType::Holding);
	input.RegisterKeyboardCommand(std::make_unique<MoveInDirection>(go.get(), glm::vec2(1.0f, 0.0f)), SDL_SCANCODE_D, dae::InputManager::ActivationType::Holding);

	input.RegisterKeyboardCommand(std::make_unique<DamageCharacter>(go.get(), 1), SDL_SCANCODE_H, dae::InputManager::ActivationType::Pressing);
	input.RegisterKeyboardCommand(std::make_unique<IncreaseScore>(go.get(), 100), SDL_SCANCODE_J, dae::InputManager::ActivationType::Pressing);

	scene.Add(std::move(go));

	// Controller Peter Pepper

	input.AddController(0);

	go = std::make_unique<dae::GameObject>();
	go->SetLocalPosition(400.0f, 300.0f);
	go->AddComponent<dae::Texture2DComponent>("PeterPepper.png");
	go->AddComponent<dae::PeterPepperComponent>(300.0f);

	input.RegisterControllerCommand(std::make_unique<MoveInDirection>(go.get(), glm::vec2(0.0f, -1.0f)), XINPUT_GAMEPAD_DPAD_UP, dae::InputManager::ActivationType::Holding, 0);
	input.RegisterControllerCommand(std::make_unique<MoveInDirection>(go.get(), glm::vec2(-1.0f, 0.0f)), XINPUT_GAMEPAD_DPAD_LEFT, dae::InputManager::ActivationType::Holding, 0);
	input.RegisterControllerCommand(std::make_unique<MoveInDirection>(go.get(), glm::vec2(0.0f, 1.0f)), XINPUT_GAMEPAD_DPAD_DOWN, dae::InputManager::ActivationType::Holding, 0);
	input.RegisterControllerCommand(std::make_unique<MoveInDirection>(go.get(), glm::vec2(1.0f, 0.0f)), XINPUT_GAMEPAD_DPAD_RIGHT, dae::InputManager::ActivationType::Holding, 0);

	scene.Add(std::move(go));
}

int main(int, char* []) {
	dae::Minigin engine("../Data/");
	engine.Run(load);
	return 0;
}