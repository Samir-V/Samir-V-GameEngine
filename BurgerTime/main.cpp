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
#include "RectCollider2DComponent.h"
#include "Scene.h"
#include "SDLSoundSystem.h"
#include "ServiceLocator.h"
#include "SpritesheetComponent.h"
#include "Texture2DComponent.h"
#include "Windows.h"
#include "Xinput.h"

#include <fstream>

#include <nlohmann/json.hpp>

#include "RotatorComponent.h"

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

using json = nlohmann::json;
using ComponentFactory = std::function<void(dae::GameObject*, const json&)>;
std::unordered_map<std::string, ComponentFactory> g_ComponentFactories;

void register_factories()
{
	g_ComponentFactories["Texture2DComponent"] =
		[](dae::GameObject* go, const json& args)
		{
			go->AddComponent<dae::Texture2DComponent>(
				args.at("texturePath").get<std::string>()
			);
		};

	g_ComponentFactories["RectCollider2DComponent"] =
		[](dae::GameObject* go, const json& args)
		{
			auto collider = go->AddComponent<dae::RectCollider2DComponent>(
				args.at("width").get<float>(),
				args.at("height").get<float>()
			);

			if (args.value("shouldCollide", false))
			{
				collider->SetShouldCollide(true);
			}

			if (args.value("shouldTriggerEvents", false))
			{
				collider->SetShouldTriggerEvents(true);
			}

			if (args.value("isStatic", false))
			{
				collider->SetIsStatic(true);
			}
		};

	g_ComponentFactories["BurgerPartComponent"] =
		[](dae::GameObject* go, const json& args)
		{
			auto sprite = args.at("spritePath").get<std::string>();
			auto slices = args.at("sliceCount").get<int>();
			go->AddComponent<dae::BurgerPartComponent>(sprite, slices);
		};
}


void load_level_from_json(const std::string& filename, dae::Scene& scene)
{
	json doc = json::parse(std::ifstream(filename));

	auto& lvl = doc.at("levels").at(0);
	for (auto& goData : lvl.at("gameObjects"))
	{
		auto go = std::make_unique<dae::GameObject>();

		go->SetTag(goData.at("tag").get<uint32_t>());

		for (auto& compData : goData.at("components"))
		{
			std::string type = compData.at("type").get<std::string>();
			const json& args = compData.at("args");

			auto it = g_ComponentFactories.find(type);
			if (it != g_ComponentFactories.end())
			{
				it->second(go.get(), args);
			}
			else
			{
				throw std::runtime_error("Unknown component type: " + type);
			}
		}

		auto pos = goData.at("position");
		go->SetWorldPosition(pos[0].get<float>(), pos[1].get<float>());

		scene.Add(std::move(go));
	}
}

void load()
{
	ServiceLocator::RegisterSoundSystem(std::make_unique<SDLSoundSystem>());

	auto& scene = dae::SceneManager::GetInstance().CreateScene("MainScene");
	auto& input = dae::InputManager::GetInstance();

	register_factories();
	load_level_from_json("levels.json", scene);

	auto go = std::make_unique<dae::GameObject>();
	auto font = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 20);
	auto textComp = go->AddComponent<dae::TextComponent>("Use D-PAD to move Peter Pepper", font);
	textComp->SetLocalPosition(5, 40);
	textComp = go->AddComponent<dae::TextComponent>("Stepping on burger parts, them falling and landing - produce sounds", font);
	textComp->SetLocalPosition(5, 70);
	scene.Add(std::move(go));


	// Keyboard Peter Pepper
	go = std::make_unique<dae::GameObject>();
	go->SetLocalPosition(200.0f, 265.0f);
	go->SetTag(make_sdbm_hash("Player"));
	auto spriteSheetComp = go->AddComponent<dae::SpritesheetComponent>("PeterPepper");

	spriteSheetComp->AddSprite("PPWalkingDown.png", make_sdbm_hash("PPWalkingDown") ,dae::SpritesheetComponent::SpriteMetaData(2, 0, 0.12f));
	spriteSheetComp->AddSprite("PPWalkingUp.png", make_sdbm_hash("PPWalkingUp"), dae::SpritesheetComponent::SpriteMetaData(2, 0, 0.12f));
	spriteSheetComp->AddSprite("PPWalkingLeft.png", make_sdbm_hash("PPWalkingLeft"), dae::SpritesheetComponent::SpriteMetaData(3, 0, 0.12f));
	spriteSheetComp->AddSprite("PPWalkingRight.png", make_sdbm_hash("PPWalkingRight"), dae::SpritesheetComponent::SpriteMetaData(3, 0, 0.12f));
	spriteSheetComp->AddSprite("PPIdleDown.png", make_sdbm_hash("PPIdleDown"), dae::SpritesheetComponent::SpriteMetaData(1, 0, 0.0f));
	spriteSheetComp->AddSprite("PPIdleUp.png", make_sdbm_hash("PPIdleUp"), dae::SpritesheetComponent::SpriteMetaData(1, 0, 0.0f));
	spriteSheetComp->AddSprite("PPSprayDown.png", make_sdbm_hash("PPSprayDown"), dae::SpritesheetComponent::SpriteMetaData(1, 0, 0.0f));
	spriteSheetComp->AddSprite("PPSprayUp.png", make_sdbm_hash("PPSprayUp"), dae::SpritesheetComponent::SpriteMetaData(1, 0, 0.0f));
	spriteSheetComp->AddSprite("PPSprayLeft.png", make_sdbm_hash("PPSprayLeft"), dae::SpritesheetComponent::SpriteMetaData(1, 0, 0.0f));
	spriteSheetComp->AddSprite("PPSprayRight.png", make_sdbm_hash("PPSprayRight"), dae::SpritesheetComponent::SpriteMetaData(1, 0, 0.0f));
	spriteSheetComp->Play(make_sdbm_hash("PPIdleDown"));

	go->AddComponent<dae::PeterPepperComponent>();

	auto rectColliderComp = go->AddComponent<dae::RectCollider2DComponent>(16.0f, 16.0f);
	auto moveComp = go->AddComponent<dae::MoveComponent>(100.0f);

	rectColliderComp->GetCollisionEnterEvent()->AddObserver(moveComp);
	rectColliderComp->GetCollisionExitEvent()->AddObserver(moveComp);
	rectColliderComp->SetShouldTriggerEvents(true);
	rectColliderComp->SetShouldCollide(true);

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

	input.RegisterKeyboardCommand(std::make_unique<SprayPepper>(go.get()), SDL_SCANCODE_P, dae::InputManager::ActivationType::Pressing);

	dae::GameObject* parentPtr = go.get();

	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->SetParent(parentPtr, false);
	go->SetLocalPosition(16.0f, 0.0f);
	go->SetTag(make_sdbm_hash("PepperSpray"));

	spriteSheetComp = go->AddComponent<dae::SpritesheetComponent>("PeterPepper");
	spriteSheetComp->AddSprite("SprayDown.png", make_sdbm_hash("SprayDown"), dae::SpritesheetComponent::SpriteMetaData(4, 0, 0.25f));
	spriteSheetComp->AddSprite("SprayUp.png", make_sdbm_hash("SprayUp"), dae::SpritesheetComponent::SpriteMetaData(4, 0, 0.25f));
	spriteSheetComp->AddSprite("SpraySideways.png", make_sdbm_hash("SpraySideways"), dae::SpritesheetComponent::SpriteMetaData(4, 0, 0.25f));
	spriteSheetComp->Play(make_sdbm_hash("SpraySideways"));

	rectColliderComp = go->AddComponent<dae::RectCollider2DComponent>(16.0f, 16.0f);
	rectColliderComp->SetShouldTriggerEvents(true);

	go->SetIsActive(false);

	scene.Add(std::move(go));

	// Controller Peter Pepper

	/*input.AddController(0);

	go = std::make_unique<dae::GameObject>();
	go->SetLocalPosition(200, 265);
	go->SetTag(make_sdbm_hash("Player"));

	auto spriteSheetComp = go->AddComponent<dae::SpritesheetComponent>("PeterPepper");

	spriteSheetComp->AddSprite("PPWalkingDown.png", make_sdbm_hash("PPWalkingDown"), dae::SpritesheetComponent::SpriteMetaData(2, 0, 0.12f));
	spriteSheetComp->AddSprite("PPWalkingUp.png", make_sdbm_hash("PPWalkingUp"), dae::SpritesheetComponent::SpriteMetaData(2, 0, 0.12f));
	spriteSheetComp->AddSprite("PPWalkingLeft.png", make_sdbm_hash("PPWalkingLeft"), dae::SpritesheetComponent::SpriteMetaData(3, 0, 0.12f));
	spriteSheetComp->AddSprite("PPWalkingRight.png", make_sdbm_hash("PPWalkingRight"), dae::SpritesheetComponent::SpriteMetaData(3, 0, 0.12f));
	spriteSheetComp->AddSprite("PPIdleDown.png", make_sdbm_hash("PPIdleDown"), dae::SpritesheetComponent::SpriteMetaData(1, 0, 0.0f));
	spriteSheetComp->AddSprite("PPIdleUp.png", make_sdbm_hash("PPIdleUp"), dae::SpritesheetComponent::SpriteMetaData(1, 0, 0.0f));
	spriteSheetComp->AddSprite("PPIdleLeft.png", make_sdbm_hash("PPIdleLeft"), dae::SpritesheetComponent::SpriteMetaData(1, 0, 0.0f));
	spriteSheetComp->AddSprite("PPIdleRight.png", make_sdbm_hash("PPIdleRight"), dae::SpritesheetComponent::SpriteMetaData(1, 0, 0.0f));
	spriteSheetComp->Play(make_sdbm_hash("PPIdleDown"));

	auto rectColliderCompController = go->AddComponent<dae::RectCollider2DComponent>(16.0f, 16.0f);
	auto moveCompController = go->AddComponent<dae::MoveComponent>(100.0f);

	rectColliderCompController->GetCollisionEnterEvent()->AddObserver(moveCompController);
	rectColliderCompController->GetCollisionExitEvent()->AddObserver(moveCompController);
	rectColliderCompController->SetShouldTriggerEvents(true);
	rectColliderCompController->SetShouldCollide(true);*/

	//const auto peterPepperCompController = go->AddComponent<dae::PeterPepperComponent>();

	// Subscribing to event
	/*peterPepperCompController->GetObjectDeathEvent()->AddObserver(healthDisplayCompController);
	peterPepperCompController->GetEnemyKilledEvent()->AddObserver(scoreComponentController);*/


	/*input.RegisterControllerCommand(std::make_unique<MoveInDirection>(go.get(), glm::vec2(0.0f, -1.0f)), XINPUT_GAMEPAD_DPAD_UP, dae::InputManager::ActivationType::Holding, 0);
	input.RegisterControllerCommand(std::make_unique<MoveInDirection>(go.get(), glm::vec2(-1.0f, 0.0f)), XINPUT_GAMEPAD_DPAD_LEFT, dae::InputManager::ActivationType::Holding, 0);
	input.RegisterControllerCommand(std::make_unique<MoveInDirection>(go.get(), glm::vec2(0.0f, 1.0f)), XINPUT_GAMEPAD_DPAD_DOWN, dae::InputManager::ActivationType::Holding, 0);
	input.RegisterControllerCommand(std::make_unique<MoveInDirection>(go.get(), glm::vec2(1.0f, 0.0f)), XINPUT_GAMEPAD_DPAD_RIGHT, dae::InputManager::ActivationType::Holding, 0);*/

	//input.RegisterControllerCommand(std::make_unique<DamageCharacter>(go.get(), 1), XINPUT_GAMEPAD_X, dae::InputManager::ActivationType::Pressing, 0);
	//input.RegisterControllerCommand(std::make_unique<KillSmallEnemy>(go.get()), XINPUT_GAMEPAD_A, dae::InputManager::ActivationType::Pressing, 0);
	//input.RegisterControllerCommand(std::make_unique<KillEnemy>(go.get()), XINPUT_GAMEPAD_B, dae::InputManager::ActivationType::Pressing, 0);

	//scene.Add(std::move(go));



	// Level temporary addition

	go = std::make_unique<dae::GameObject>();
	go->SetLocalPosition(200.0f, 280.0f);
	go->AddComponent<dae::Texture2DComponent>("Level/WidePlatform.png");
	go->AddComponent<dae::RectCollider2DComponent>(32.0f, 4.0f);

	go->SetTag(make_sdbm_hash("Platform"));
	scene.Add(std::move(go));
	go = std::make_unique<dae::GameObject>();
	go->SetLocalPosition(232.0f, 280.0f);
	go->AddComponent<dae::Texture2DComponent>("Level/WidePlatform.png");
	go->AddComponent<dae::RectCollider2DComponent>(32.0f, 4.0f);
	go->SetTag(make_sdbm_hash("Platform"));
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->SetLocalPosition(264.0f, 280.0f);
	go->AddComponent<dae::Texture2DComponent>("Level/WidePlatform.png");
	go->AddComponent<dae::RectCollider2DComponent>(32.0f, 4.0f);
	go->SetTag(make_sdbm_hash("Platform"));
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->SetLocalPosition(243.0f, 263.0f);
	go->AddComponent<dae::Texture2DComponent>("Level/Ladder.png");
	go->AddComponent<dae::RectCollider2DComponent>(10.0f, 17.0f);
	go->SetTag(make_sdbm_hash("Ladder"));
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->SetLocalPosition(243.0f, 247.0f);
	go->AddComponent<dae::Texture2DComponent>("Level/Ladder.png");
	go->AddComponent<dae::RectCollider2DComponent>(10.0f, 16.0f);
	go->SetTag(make_sdbm_hash("Ladder"));
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->SetLocalPosition(200.0f, 243.0f);
	go->AddComponent<dae::Texture2DComponent>("Level/WidePlatform.png");
	go->AddComponent<dae::RectCollider2DComponent>(32.0f, 4.0f);
	go->SetTag(make_sdbm_hash("Platform"));
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->SetLocalPosition(232.0f, 243.0f);
	go->AddComponent<dae::Texture2DComponent>("Level/WidePlatform.png");
	go->AddComponent<dae::RectCollider2DComponent>(32.0f, 4.0f);
	go->SetTag(make_sdbm_hash("Platform"));
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->SetLocalPosition(264.0f, 243.0f);
	go->AddComponent<dae::Texture2DComponent>("Level/WidePlatform.png");
	go->AddComponent<dae::RectCollider2DComponent>(32.0f, 4.0f);
	go->SetTag(make_sdbm_hash("Platform"));
	scene.Add(std::move(go));

	// -------------------
	/*go = std::make_unique<dae::GameObject>();
	go->SetLocalPosition(296.0f, 243.0f);
	go->AddComponent<dae::Texture2DComponent>("Level/WidePlatform.png");
	go->AddComponent<dae::RectCollider2DComponent>(32.0f, 4.0f);

	go->SetTag(make_sdbm_hash("Platform"));
	scene.Add(std::move(go));


	go = std::make_unique<dae::GameObject>();
	go->SetLocalPosition(296.0f, 280.0f);
	go->AddComponent<dae::Texture2DComponent>("Level/WidePlatform.png");
	go->AddComponent<dae::RectCollider2DComponent>(32.0f, 4.0f);

	go->SetTag(make_sdbm_hash("Platform"));
	scene.Add(std::move(go));*/

	/*go = std::make_unique<dae::GameObject>();
	go->SetLocalPosition(296.0f, 241.0f);
	go->SetTag(make_sdbm_hash("BurgerPart"));
	auto burgerPartComp = go->AddComponent<dae::BurgerPartComponent>("BurgerParts/Burger.png", 8);
	auto partCollider = go->AddComponent<dae::RectCollider2DComponent>(32.0f, 7.0f);

	partCollider->GetCollisionEnterEvent()->AddObserver(burgerPartComp);
	partCollider->GetCollisionStayEvent()->AddObserver(burgerPartComp);
	partCollider->GetCollisionExitEvent()->AddObserver(burgerPartComp);
	partCollider->SetShouldTriggerEvents(true);

	scene.Add(std::move(go));*/


	go = std::make_unique<dae::GameObject>();
	go->SetLocalPosition(261.0f, 310.0f);
	go->AddComponent<dae::Texture2DComponent>("Level/Plate.png");
	go->AddComponent<dae::RectCollider2DComponent>(38.0f, 5.0f);

	go->SetTag(make_sdbm_hash("Plate"));
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->SetLocalPosition(264.0f, 241.0f);
	go->SetTag(make_sdbm_hash("BurgerPart"));
	auto burgerPartComp = go->AddComponent<dae::BurgerPartComponent>("BurgerParts/Burger.png", 8);
	auto partCollider = go->AddComponent<dae::RectCollider2DComponent>(32.0f, 7.0f);

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

	partCollider->GetCollisionEnterEvent()->AddObserver(burgerPartComp);
	partCollider->GetCollisionStayEvent()->AddObserver(burgerPartComp);
	partCollider->GetCollisionExitEvent()->AddObserver(burgerPartComp);
	partCollider->SetShouldTriggerEvents(true);

	scene.Add(std::move(go));


	// Invisible walls
	go = std::make_unique<dae::GameObject>();
	go->SetLocalPosition(332.0f, 220.0f);
	partCollider = go->AddComponent<dae::RectCollider2DComponent>(4.0f, 100.0f);
	partCollider->SetShouldCollide(true);
	partCollider->SetIsStatic(true);

	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->SetLocalPosition(196.0f, 220.0f);
	partCollider = go->AddComponent<dae::RectCollider2DComponent>(4.0f, 100.0f);
	partCollider->SetShouldCollide(true);
	partCollider->SetIsStatic(true);

	scene.Add(std::move(go));


	// Sound addition

	auto& sound = ServiceLocator::GetSoundSystem();

	sound.SetFolder("Sound");

	sound.LoadSound("BurgerStep.wav");
	sound.LoadSound("BurgerLand.wav");
	sound.LoadSound("BurgerFall.wav");
	sound.LoadSound("MainTheme.mp3", true);
	sound.Play("MainTheme.mp3", 0.8f, true);
}

int main(int, char* []) {
	find_resources();
	dae::Minigin engine{};
	engine.Run(load);
	return 0;
}