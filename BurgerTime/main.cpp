#include <SDL.h>

#if _DEBUG
// ReSharper disable once CppUnusedIncludeDirective
#if __has_include(<vld.h>)
#include <vld.h>
#endif
#endif

#include "BurgerPartComponent.h"
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
#include "RectCollider2DComponent.h"
#include "Scene.h"
#include "ScoreComponent.h"
#include "SDLSoundSystem.h"
#include "ServiceLocator.h"
#include "SpritesheetComponent.h"
#include "Texture2DComponent.h"
#include "Windows.h"
#include "Xinput.h"

void find_resources()
{
	constexpr int MAX_TRAVERSAL{ 3 };

	const std::filesystem::path resFolderName{ "Data" };
	int counter{ 0 };

	while (!std::filesystem::exists(resFolderName) && counter < MAX_TRAVERSAL)
	{
		std::filesystem::current_path("..");
		counter++;
	}

	std::filesystem::current_path(resFolderName);
}

void load()
{
	ServiceLocator::RegisterSoundSystem(std::make_unique<SDLSoundSystem>());

	auto& scene = dae::SceneManager::GetInstance().CreateScene("MainScene");
	auto& input = dae::InputManager::GetInstance();

	/*auto go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("background.tga");
	scene.Add(std::move(go));*/

	/*go = std::make_unique<dae::GameObject>();
	const auto textureComp = go->AddComponent<dae::Texture2DComponent>("logo.tga");
	textureComp->SetLocalPosition(216, 180);
	scene.Add(std::move(go));*/

	/*go = std::make_unique<dae::GameObject>();
	auto font = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 28);
	auto textComp = go->AddComponent<dae::TextComponent>("Programming 4 Assignment", font);
	textComp->SetLocalPosition(140, 40);
	scene.Add(std::move(go));*/

	// FPS display
	/*go = std::make_unique<dae::GameObject>();
	textComp = go->AddComponent<dae::TextComponent>("60.0 FPS", font);
	textComp->SetLocalPosition(10, 10);
	go->AddComponent<dae::FPSComponent>(textComp);
	scene.Add(std::move(go));

	auto displaysFont = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 16);*/

	// Keyboard how to play
	/*go = std::make_unique<dae::GameObject>();
	textComp = go->AddComponent<dae::TextComponent>("Use WASD to move left Peter Pepper, H to inflict damage, J and K to increase score", displaysFont);
	textComp->SetLocalPosition(20, 100);
	scene.Add(std::move(go));*/

	// Controller how to play
	/*go = std::make_unique<dae::GameObject>();
	textComp = go->AddComponent<dae::TextComponent>("Use DPAD to move right Peter Pepper, X to inflict damage, A and B to increase score", displaysFont);
	textComp->SetLocalPosition(20, 120);
	scene.Add(std::move(go));*/

	// Health Display
	/*auto healthDisplayGO = std::make_unique<dae::GameObject>();

	textComp = healthDisplayGO->AddComponent<dae::TextComponent>("Remaining Health: 3", displaysFont);
	const auto healthDisplayCompKeyboard = healthDisplayGO->AddComponent<dae::HealthDisplayComponent>(textComp);
	healthDisplayGO->SetLocalPosition(20.0f, 165.0f);

	scene.Add(std::move(healthDisplayGO));*/

	// Score Display
	/*auto scoreDisplayGO = std::make_unique<dae::GameObject>();

	textComp = scoreDisplayGO->AddComponent<dae::TextComponent>("Score: 0", displaysFont);
	scoreDisplayGO->SetLocalPosition(20.0f, 150.0f);
	scene.Add(std::move(scoreDisplayGO));*/


	// Keyboard Peter Pepper
	auto go = std::make_unique<dae::GameObject>();
	go->SetLocalPosition(200.0f, 265.0f);
	go->SetTag(make_sdbm_hash("Player"));
	const auto spriteSheetComp = go->AddComponent<dae::SpritesheetComponent>("PeterPepper");

	spriteSheetComp->AddSprite("PPWalkingDown.png", make_sdbm_hash("PPWalkingDown") ,dae::SpritesheetComponent::SpriteMetaData(2, 0, 0.12f));
	spriteSheetComp->AddSprite("PPWalkingUp.png", make_sdbm_hash("PPWalkingUp"), dae::SpritesheetComponent::SpriteMetaData(2, 0, 0.12f));
	spriteSheetComp->AddSprite("PPWalkingLeft.png", make_sdbm_hash("PPWalkingLeft"), dae::SpritesheetComponent::SpriteMetaData(3, 0, 0.12f));
	spriteSheetComp->AddSprite("PPWalkingRight.png", make_sdbm_hash("PPWalkingRight"), dae::SpritesheetComponent::SpriteMetaData(3, 0, 0.12f));
	spriteSheetComp->AddSprite("PPIdleDown.png", make_sdbm_hash("PPIdleDown"), dae::SpritesheetComponent::SpriteMetaData(1, 0, 0.0f));
	spriteSheetComp->AddSprite("PPIdleUp.png", make_sdbm_hash("PPIdleUp"), dae::SpritesheetComponent::SpriteMetaData(1, 0, 0.0f));
	spriteSheetComp->AddSprite("PPIdleLeft.png", make_sdbm_hash("PPIdleLeft"),  dae::SpritesheetComponent::SpriteMetaData(1, 0, 0.0f));
	spriteSheetComp->AddSprite("PPIdleRight.png", make_sdbm_hash("PPIdleRight"), dae::SpritesheetComponent::SpriteMetaData(1, 0, 0.0f));
	spriteSheetComp->Play(make_sdbm_hash("PPIdleDown"));

	//const auto peterPepperComp = go->AddComponent<dae::PeterPepperComponent>();



	const auto rectColliderComp = go->AddComponent<dae::RectCollider2DComponent>(16.0f, 16.0f);
	const auto moveComp = go->AddComponent<dae::MoveComponent>(100.0f);

	rectColliderComp->GetCollisionEnterEvent()->AddObserver(moveComp);
	rectColliderComp->GetCollisionExitEvent()->AddObserver(moveComp);
	rectColliderComp->SetShouldTriggerEvents(true);



	//const auto scoreComponentKeyboard = go->AddComponent<dae::ScoreComponent>(textComp);

	// Subscribing to event
	/*peterPepperComp->GetObjectDeathEvent()->AddObserver(healthDisplayCompKeyboard);
	peterPepperComp->GetEnemyKilledEvent()->AddObserver(scoreComponentKeyboard);*/

	input.RegisterKeyboardCommand(std::make_unique<MoveInDirection>(go.get(), glm::vec2(0.0f, -1.0f)), SDL_SCANCODE_W, dae::InputManager::ActivationType::Holding);
	input.RegisterKeyboardCommand(std::make_unique<MoveInDirection>(go.get(), glm::vec2(-1.0f, 0.0f)), SDL_SCANCODE_A, dae::InputManager::ActivationType::Holding);
	input.RegisterKeyboardCommand(std::make_unique<MoveInDirection>(go.get(), glm::vec2(0.0f, 1.0f)), SDL_SCANCODE_S, dae::InputManager::ActivationType::Holding);
	input.RegisterKeyboardCommand(std::make_unique<MoveInDirection>(go.get(), glm::vec2(1.0f, 0.0f)), SDL_SCANCODE_D, dae::InputManager::ActivationType::Holding);

	input.RegisterKeyboardCommand(std::make_unique<DamageCharacter>(go.get(), 1), SDL_SCANCODE_H, dae::InputManager::ActivationType::Pressing);
	input.RegisterKeyboardCommand(std::make_unique<KillSmallEnemy>(go.get()), SDL_SCANCODE_J, dae::InputManager::ActivationType::Pressing);
	input.RegisterKeyboardCommand(std::make_unique<KillEnemy>(go.get()), SDL_SCANCODE_K, dae::InputManager::ActivationType::Pressing);

	scene.Add(std::move(go));


	// Health Display for Controller Peter Pepper

	/*auto healthDisplayGOController = std::make_unique<dae::GameObject>();

	textComp = healthDisplayGOController->AddComponent<dae::TextComponent>("Remaining Health: 3", displaysFont);
	const auto healthDisplayCompController = healthDisplayGOController->AddComponent<dae::HealthDisplayComponent>(textComp);
	healthDisplayGOController->SetLocalPosition(20.0f, 215.0f);

	scene.Add(std::move(healthDisplayGOController));*/

	// Score Display

	/*auto scoreDisplayGOController = std::make_unique<dae::GameObject>();

	textComp = scoreDisplayGOController->AddComponent<dae::TextComponent>("Score: 0", displaysFont);
	scoreDisplayGOController->SetLocalPosition(20.0f, 200.0f);

	scene.Add(std::move(scoreDisplayGOController));*/

	// Controller Peter Pepper

	input.AddController(0);

	go = std::make_unique<dae::GameObject>();
	go->SetLocalPosition(400.0f, 300.0f);
	go->AddComponent<dae::Texture2DComponent>("PeterPepper.png");
	const auto rectColliderCompController = go->AddComponent<dae::RectCollider2DComponent>(16.0f, 16.0f);
	rectColliderCompController->SetIsStatic(true);
	//const auto peterPepperCompController = go->AddComponent<dae::PeterPepperComponent>();

	//const auto scoreComponentController = go->AddComponent<dae::ScoreComponent>(textComp);

	// Subscribing to event
	/*peterPepperCompController->GetObjectDeathEvent()->AddObserver(healthDisplayCompController);
	peterPepperCompController->GetEnemyKilledEvent()->AddObserver(scoreComponentController);*/


	input.RegisterControllerCommand(std::make_unique<MoveInDirection>(go.get(), glm::vec2(0.0f, -1.0f)), XINPUT_GAMEPAD_DPAD_UP, dae::InputManager::ActivationType::Holding, 0);
	input.RegisterControllerCommand(std::make_unique<MoveInDirection>(go.get(), glm::vec2(-1.0f, 0.0f)), XINPUT_GAMEPAD_DPAD_LEFT, dae::InputManager::ActivationType::Holding, 0);
	input.RegisterControllerCommand(std::make_unique<MoveInDirection>(go.get(), glm::vec2(0.0f, 1.0f)), XINPUT_GAMEPAD_DPAD_DOWN, dae::InputManager::ActivationType::Holding, 0);
	input.RegisterControllerCommand(std::make_unique<MoveInDirection>(go.get(), glm::vec2(1.0f, 0.0f)), XINPUT_GAMEPAD_DPAD_RIGHT, dae::InputManager::ActivationType::Holding, 0);

	input.RegisterControllerCommand(std::make_unique<DamageCharacter>(go.get(), 1), XINPUT_GAMEPAD_X, dae::InputManager::ActivationType::Pressing, 0);
	input.RegisterControllerCommand(std::make_unique<KillSmallEnemy>(go.get()), XINPUT_GAMEPAD_A, dae::InputManager::ActivationType::Pressing, 0);
	input.RegisterControllerCommand(std::make_unique<KillEnemy>(go.get()), XINPUT_GAMEPAD_B, dae::InputManager::ActivationType::Pressing, 0);

	scene.Add(std::move(go));



	// Level addition

	go = std::make_unique<dae::GameObject>();
	go->SetLocalPosition(200.0f, 280.0f);
	go->AddComponent<dae::Texture2DComponent>("Level/WidePlatform.png");
	auto partCollider = go->AddComponent<dae::RectCollider2DComponent>(32.0f, 4.0f);
	partCollider->SetShouldCollide(false);

	go->SetTag(make_sdbm_hash("Platform"));
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->SetLocalPosition(232.0f, 280.0f);
	go->AddComponent<dae::Texture2DComponent>("Level/WidePlatform.png");
	partCollider = go->AddComponent<dae::RectCollider2DComponent>(32.0f, 4.0f);
	partCollider->SetShouldCollide(false);

	go->SetTag(make_sdbm_hash("Platform"));
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->SetLocalPosition(264.0f, 280.0f);
	go->AddComponent<dae::Texture2DComponent>("Level/WidePlatform.png");
	partCollider = go->AddComponent<dae::RectCollider2DComponent>(32.0f, 4.0f);
	partCollider->SetShouldCollide(false);

	go->SetTag(make_sdbm_hash("Platform"));
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->SetLocalPosition(243.0f, 263.0f);
	go->AddComponent<dae::Texture2DComponent>("Level/Ladder.png");
	partCollider = go->AddComponent<dae::RectCollider2DComponent>(10.0f, 17.0f);
	partCollider->SetShouldCollide(false);

	go->SetTag(make_sdbm_hash("Ladder"));
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->SetLocalPosition(243.0f, 247.0f);
	go->AddComponent<dae::Texture2DComponent>("Level/Ladder.png");
	partCollider = go->AddComponent<dae::RectCollider2DComponent>(10.0f, 16.0f);
	partCollider->SetShouldCollide(false);

	go->SetTag(make_sdbm_hash("Ladder"));
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->SetLocalPosition(200.0f, 243.0f);
	go->AddComponent<dae::Texture2DComponent>("Level/WidePlatform.png");
	partCollider = go->AddComponent<dae::RectCollider2DComponent>(32.0f, 4.0f);
	partCollider->SetShouldCollide(false);

	go->SetTag(make_sdbm_hash("Platform"));
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->SetLocalPosition(232.0f, 243.0f);
	go->AddComponent<dae::Texture2DComponent>("Level/WidePlatform.png");
	partCollider = go->AddComponent<dae::RectCollider2DComponent>(32.0f, 4.0f);
	partCollider->SetShouldCollide(false);

	go->SetTag(make_sdbm_hash("Platform"));
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->SetLocalPosition(264.0f, 243.0f);
	go->AddComponent<dae::Texture2DComponent>("Level/WidePlatform.png");
	partCollider = go->AddComponent<dae::RectCollider2DComponent>(32.0f, 4.0f);
	partCollider->SetShouldCollide(false);

	go->SetTag(make_sdbm_hash("Platform"));
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->SetLocalPosition(261.0f, 310.0f);
	go->AddComponent<dae::Texture2DComponent>("Level/Plate.png");
	partCollider = go->AddComponent<dae::RectCollider2DComponent>(38.0f, 5.0f);
	partCollider->SetShouldCollide(false);

	go->SetTag(make_sdbm_hash("Plate"));
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->SetLocalPosition(264.0f, 241.0f);
	go->SetTag(make_sdbm_hash("BurgerPart"));
	auto burgerPartComp = go->AddComponent<dae::BurgerPartComponent>("BurgerParts/Burger.png", 8);
	partCollider = go->AddComponent<dae::RectCollider2DComponent>(32.0f, 7.0f);
	partCollider->SetShouldCollide(false);

	partCollider->GetCollisionEnterEvent()->AddObserver(burgerPartComp);
	partCollider->GetCollisionStayEvent()->AddObserver(burgerPartComp);
	partCollider->GetCollisionExitEvent()->AddObserver(burgerPartComp);
	partCollider->SetShouldTriggerEvents(true);

	scene.Add(std::move(go));


	go = std::make_unique<dae::GameObject>();
	go->SetLocalPosition(264.0f, 278.0f);
	go->SetTag(make_sdbm_hash("BurgerPart"));
	burgerPartComp = go->AddComponent<dae::BurgerPartComponent>("BurgerParts/BurgerBunBottom.png", 8);
	partCollider = go->AddComponent<dae::RectCollider2DComponent>(32.0f, 7.0f);
	partCollider->SetShouldCollide(false);

	partCollider->GetCollisionEnterEvent()->AddObserver(burgerPartComp);
	partCollider->GetCollisionStayEvent()->AddObserver(burgerPartComp);
	partCollider->GetCollisionExitEvent()->AddObserver(burgerPartComp);
	partCollider->SetShouldTriggerEvents(true);

	scene.Add(std::move(go));
}

int main(int, char* []) {
	find_resources();
	dae::Minigin engine{};
	engine.Run(load);
	return 0;
}