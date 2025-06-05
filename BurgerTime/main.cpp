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

#include "EnemyComponent.h"
#include "EnemyMoveComponent.h"
#include "GameHandlerComponent.h"
#include "MenuCommand.h"
#include "MenuHUDComponent.h"
#include "ScoreComponent.h"

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

	auto& input = dae::InputManager::GetInstance();
	input.AddInputMap(make_sdbm_hash("MenuMap"));
	input.AddInputMap(make_sdbm_hash("GameplayMap"));
	input.SetActiveInputMap(make_sdbm_hash("MenuMap"));



	auto& sceneDontDestroy = dae::SceneManager::GetInstance().CreateScene("DontDestroyOnLoadScene", true);
	auto go = std::make_unique<dae::GameObject>();
	auto gameHandlerComp = go->AddComponent<dae::GameHandlerComponent>();
	go->SetTag(make_sdbm_hash("GameHandler"));
	sceneDontDestroy.Add(std::move(go));

	// Menu scene
	auto& menuScene = dae::SceneManager::GetInstance().CreateScene("Menu", false);
	auto fontBig = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 32);
	auto font = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 20);

	go = std::make_unique<dae::GameObject>();

	auto textComp = go->AddComponent<dae::TextComponent>("Burger Time!", fontBig);
	textComp->SetLocalPosition(230, 40);

	auto textComp1 = go->AddComponent<dae::TextComponent>("Solo mode", font);
	textComp1->SetLocalPosition(275, 110);

	auto textComp2 = go->AddComponent<dae::TextComponent>("Co-op mode", font);
	textComp2->SetLocalPosition(275, 160);

	auto textComp3 = go->AddComponent<dae::TextComponent>("Versus mode", font);
	textComp3->SetLocalPosition(275, 210);


	textComp = go->AddComponent<dae::TextComponent>("--->", font);
	textComp->SetLocalPosition(230, 110);

	go->AddComponent<dae::MenuHUDComponent>(textComp, std::vector<dae::TextComponent*>{{textComp1, textComp2, textComp3}});

	input.RegisterControllerCommand(make_sdbm_hash("MenuMap"), std::make_unique<dae::AlterHUDCounter>(go.get(), -1), XINPUT_GAMEPAD_DPAD_UP, dae::InputManager::ActivationType::Pressing, 0);
	input.RegisterControllerCommand(make_sdbm_hash("MenuMap"), std::make_unique<dae::AlterHUDCounter>(go.get(), 1), XINPUT_GAMEPAD_DPAD_DOWN, dae::InputManager::ActivationType::Pressing, 0);
	input.RegisterControllerCommand(make_sdbm_hash("MenuMap"), std::make_unique<dae::ConfirmModeChoice>(go.get()), XINPUT_GAMEPAD_A, dae::InputManager::ActivationType::Pressing, 0);

	menuScene.Add(std::move(go));

	// Level 1
	auto& scene = dae::SceneManager::GetInstance().CreateScene("Level1", false);

	register_factories();
	load_level_from_json("levels.json", scene);

	go = std::make_unique<dae::GameObject>();
	textComp = go->AddComponent<dae::TextComponent>("Score: 0", font);
	textComp->SetLocalPosition(5, 40);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::ScoreComponent>(textComp);
	scene.Add(std::move(go));

	// Controller Peter Pepper

	input.AddController(0);

	go = std::make_unique<dae::GameObject>();
	go->SetTag(make_sdbm_hash("Player"));

	auto spriteSheetComp = go->AddComponent<dae::SpritesheetComponent>("PeterPepper");

	spriteSheetComp->AddSprite("PPWalkingDown.png", make_sdbm_hash("PPWalkingDown"), dae::SpritesheetComponent::SpriteMetaData(2, 0, 0.12f));
	spriteSheetComp->AddSprite("PPWalkingUp.png", make_sdbm_hash("PPWalkingUp"), dae::SpritesheetComponent::SpriteMetaData(2, 0, 0.12f));
	spriteSheetComp->AddSprite("PPWalkingLeft.png", make_sdbm_hash("PPWalkingLeft"), dae::SpritesheetComponent::SpriteMetaData(3, 0, 0.12f));
	spriteSheetComp->AddSprite("PPWalkingRight.png", make_sdbm_hash("PPWalkingRight"), dae::SpritesheetComponent::SpriteMetaData(3, 0, 0.12f));
	spriteSheetComp->AddSprite("PPIdleDown.png", make_sdbm_hash("PPIdleDown"), dae::SpritesheetComponent::SpriteMetaData(1, 0, 0.0f));
	spriteSheetComp->AddSprite("PPIdleUp.png", make_sdbm_hash("PPIdleUp"), dae::SpritesheetComponent::SpriteMetaData(1, 0, 0.0f));
	spriteSheetComp->AddSprite("PPSprayDown.png", make_sdbm_hash("PPSprayDown"), dae::SpritesheetComponent::SpriteMetaData(1, 0, 0.0f));
	spriteSheetComp->AddSprite("PPSprayUp.png", make_sdbm_hash("PPSprayUp"), dae::SpritesheetComponent::SpriteMetaData(1, 0, 0.0f));
	spriteSheetComp->AddSprite("PPSprayLeft.png", make_sdbm_hash("PPSprayLeft"), dae::SpritesheetComponent::SpriteMetaData(1, 0, 0.0f));
	spriteSheetComp->AddSprite("PPSprayRight.png", make_sdbm_hash("PPSprayRight"), dae::SpritesheetComponent::SpriteMetaData(1, 0, 0.0f));
	spriteSheetComp->AddSprite("PPDying.png", make_sdbm_hash("PPDying"), dae::SpritesheetComponent::SpriteMetaData(5, 0, 0.4f));
	spriteSheetComp->AddSprite("PPWin.png", make_sdbm_hash("PPWin"), dae::SpritesheetComponent::SpriteMetaData(2, 0, 0.3f));
	spriteSheetComp->Play(make_sdbm_hash("PPIdleDown"));

	auto rectColliderCompController = go->AddComponent<dae::RectCollider2DComponent>(16.0f, 16.0f);
	rectColliderCompController->SetShouldTriggerEvents(true);
	rectColliderCompController->SetShouldCollide(true);

	go->AddComponent<dae::MoveComponent>(55.0f);
	go->AddComponent<dae::PeterPepperComponent>();

	input.RegisterControllerCommand(make_sdbm_hash("GameplayMap"), std::make_unique<MoveInDirection>(go.get(), glm::vec2(0.0f, -1.0f)), XINPUT_GAMEPAD_DPAD_UP, dae::InputManager::ActivationType::Holding, 0);
	input.RegisterControllerCommand(make_sdbm_hash("GameplayMap"), std::make_unique<MoveInDirection>(go.get(), glm::vec2(-1.0f, 0.0f)), XINPUT_GAMEPAD_DPAD_LEFT, dae::InputManager::ActivationType::Holding, 0);
	input.RegisterControllerCommand(make_sdbm_hash("GameplayMap"), std::make_unique<MoveInDirection>(go.get(), glm::vec2(0.0f, 1.0f)), XINPUT_GAMEPAD_DPAD_DOWN, dae::InputManager::ActivationType::Holding, 0);
	input.RegisterControllerCommand(make_sdbm_hash("GameplayMap"), std::make_unique<MoveInDirection>(go.get(), glm::vec2(1.0f, 0.0f)), XINPUT_GAMEPAD_DPAD_RIGHT, dae::InputManager::ActivationType::Holding, 0);

	input.RegisterControllerCommand(make_sdbm_hash("GameplayMap"), std::make_unique<SprayPepper>(go.get()), XINPUT_GAMEPAD_X, dae::InputManager::ActivationType::Pressing, 0);

	dae::GameObject* playerPtr = go.get();

	go->SetWorldPosition(200, 265);

	scene.Add(std::move(go));


	// Pepper Spray

	go = std::make_unique<dae::GameObject>();
	go->SetParent(playerPtr, false);
	go->SetLocalPosition(16.0f, 0.0f);
	go->SetTag(make_sdbm_hash("PepperSpray"));

	spriteSheetComp = go->AddComponent<dae::SpritesheetComponent>("PeterPepper");
	spriteSheetComp->AddSprite("SprayDown.png", make_sdbm_hash("SprayDown"), dae::SpritesheetComponent::SpriteMetaData(4, 0, 0.25f));
	spriteSheetComp->AddSprite("SprayUp.png", make_sdbm_hash("SprayUp"), dae::SpritesheetComponent::SpriteMetaData(4, 0, 0.25f));
	spriteSheetComp->AddSprite("SpraySideways.png", make_sdbm_hash("SpraySideways"), dae::SpritesheetComponent::SpriteMetaData(4, 0, 0.25f));
	spriteSheetComp->Play(make_sdbm_hash("SpraySideways"));

	auto rectColliderComp = go->AddComponent<dae::RectCollider2DComponent>(16.0f, 16.0f);
	rectColliderComp->SetShouldTriggerEvents(true);

	go->SetIsActive(false);

	scene.Add(std::move(go));



	// Level temporary addition

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/WidePlatform.png");
	go->AddComponent<dae::RectCollider2DComponent>(32.0f, 4.0f);
	go->SetTag(make_sdbm_hash("Platform"));
	go->SetWorldPosition(168.0f, 316.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/Ladder.png");
	go->AddComponent<dae::RectCollider2DComponent>(10.0f, 17.0f);
	go->SetTag(make_sdbm_hash("Ladder"));
	go->SetWorldPosition(179.0f, 299.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/Ladder.png");
	go->AddComponent<dae::RectCollider2DComponent>(10.0f, 16.0f);
	go->SetTag(make_sdbm_hash("Ladder"));
	go->SetWorldPosition(179.0f, 283.0f);
	scene.Add(std::move(go));


	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/WidePlatform.png");
	go->AddComponent<dae::RectCollider2DComponent>(32.0f, 4.0f);
	go->SetTag(make_sdbm_hash("Platform"));
	go->SetWorldPosition(200.0f, 316.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/WidePlatform.png");
	go->AddComponent<dae::RectCollider2DComponent>(32.0f, 4.0f);
	go->SetTag(make_sdbm_hash("Platform"));
	go->SetWorldPosition(232.0f, 316.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/Ladder.png");
	go->AddComponent<dae::RectCollider2DComponent>(10.0f, 17.0f);
	go->SetTag(make_sdbm_hash("Ladder"));
	go->SetWorldPosition(243.0f, 299.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/Ladder.png");
	go->AddComponent<dae::RectCollider2DComponent>(10.0f, 16.0f);
	go->SetTag(make_sdbm_hash("Ladder"));
	go->SetWorldPosition(243.0f, 283.0f);
	scene.Add(std::move(go));




	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/WidePlatform.png");
	go->AddComponent<dae::RectCollider2DComponent>(32.0f, 4.0f);
	go->SetTag(make_sdbm_hash("Platform"));
	go->SetWorldPosition(168.0f, 280.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/Ladder.png");
	go->AddComponent<dae::RectCollider2DComponent>(10.0f, 17.0f);
	go->SetTag(make_sdbm_hash("Ladder"));
	go->SetWorldPosition(179.0f, 263.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/Ladder.png");
	go->AddComponent<dae::RectCollider2DComponent>(10.0f, 16.0f);
	go->SetTag(make_sdbm_hash("Ladder"));
	go->SetWorldPosition(179.0f, 247.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/WidePlatform.png");
	go->AddComponent<dae::RectCollider2DComponent>(32.0f, 4.0f);
	go->SetTag(make_sdbm_hash("Platform"));
	go->SetWorldPosition(168.0f, 243.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/WidePlatform.png");
	go->AddComponent<dae::RectCollider2DComponent>(32.0f, 4.0f);
	go->SetTag(make_sdbm_hash("Platform"));
	go->SetWorldPosition(200.0f, 280.0f);
	scene.Add(std::move(go));


	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/WidePlatform.png");
	go->AddComponent<dae::RectCollider2DComponent>(32.0f, 4.0f);
	go->SetTag(make_sdbm_hash("Platform"));
	go->SetWorldPosition(232.0f, 280.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/WidePlatform.png");
	go->AddComponent<dae::RectCollider2DComponent>(32.0f, 4.0f);
	go->SetTag(make_sdbm_hash("Platform"));
	go->SetWorldPosition(264.0f, 280.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/Ladder.png");
	go->AddComponent<dae::RectCollider2DComponent>(10.0f, 17.0f);
	go->SetTag(make_sdbm_hash("Ladder"));
	go->SetWorldPosition(243.0f, 263.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();

	go->AddComponent<dae::Texture2DComponent>("Level/Ladder.png");
	go->AddComponent<dae::RectCollider2DComponent>(10.0f, 16.0f);
	go->SetTag(make_sdbm_hash("Ladder"));
	go->SetWorldPosition(243.0f, 247.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/WidePlatform.png");
	go->AddComponent<dae::RectCollider2DComponent>(32.0f, 4.0f);
	go->SetTag(make_sdbm_hash("Platform"));
	go->SetWorldPosition(200.0f, 243.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/WidePlatform.png");
	go->AddComponent<dae::RectCollider2DComponent>(32.0f, 4.0f);
	go->SetTag(make_sdbm_hash("Platform"));
	go->SetWorldPosition(232.0f, 243.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/WidePlatform.png");
	go->AddComponent<dae::RectCollider2DComponent>(32.0f, 4.0f);
	go->SetTag(make_sdbm_hash("Platform"));
	go->SetWorldPosition(264.0f, 243.0f);
	scene.Add(std::move(go));


	// Third floor

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/Ladder.png");
	go->AddComponent<dae::RectCollider2DComponent>(10.0f, 17.0f);
	go->SetTag(make_sdbm_hash("Ladder"));
	go->SetWorldPosition(179.0f, 226.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/Ladder.png");
	go->AddComponent<dae::RectCollider2DComponent>(10.0f, 16.0f);
	go->SetTag(make_sdbm_hash("Ladder"));
	go->SetWorldPosition(179.0f, 210.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/WidePlatform.png");
	go->AddComponent<dae::RectCollider2DComponent>(32.0f, 4.0f);
	go->SetTag(make_sdbm_hash("Platform"));
	go->SetWorldPosition(168.0f, 206.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/WidePlatform.png");
	go->AddComponent<dae::RectCollider2DComponent>(32.0f, 4.0f);
	go->SetTag(make_sdbm_hash("Platform"));
	go->SetWorldPosition(200.0f, 206.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/WidePlatform.png");
	go->AddComponent<dae::RectCollider2DComponent>(32.0f, 4.0f);
	go->SetTag(make_sdbm_hash("Platform"));
	go->SetWorldPosition(232.0f, 206.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/WidePlatform.png");
	go->AddComponent<dae::RectCollider2DComponent>(32.0f, 4.0f);
	go->SetTag(make_sdbm_hash("Platform"));
	go->SetWorldPosition(264.0f, 206.0f);
	scene.Add(std::move(go));

	/*go = std::make_unique<dae::GameObject>();
	go->SetLocalPosition(243.0f, 226.0f);
	go->AddComponent<dae::Texture2DComponent>("Level/Ladder.png");
	go->AddComponent<dae::RectCollider2DComponent>(10.0f, 17.0f);
	go->SetTag(make_sdbm_hash("Ladder"));
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->SetLocalPosition(243.0f, 210.0f);
	go->AddComponent<dae::Texture2DComponent>("Level/Ladder.png");
	go->AddComponent<dae::RectCollider2DComponent>(10.0f, 16.0f);
	go->SetTag(make_sdbm_hash("Ladder"));
	scene.Add(std::move(go));*/

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
	go->SetTag(make_sdbm_hash("BurgerPart"));
go->AddComponent<dae::BurgerPartComponent>("BurgerParts/Cheese.png", 7);
	auto partCollider = go->AddComponent<dae::RectCollider2DComponent>(28.0f, 7.0f);

	partCollider->SetShouldTriggerEvents(true);

	go->SetWorldPosition(266.0f, 204.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/Plate.png");
	go->AddComponent<dae::RectCollider2DComponent>(38.0f, 5.0f);
	go->SetTag(make_sdbm_hash("Plate"));
	go->SetWorldPosition(261.0f, 310.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->SetTag(make_sdbm_hash("BurgerPart"));
	go->AddComponent<dae::BurgerPartComponent>("BurgerParts/Burger.png", 8);
	partCollider = go->AddComponent<dae::RectCollider2DComponent>(32.0f, 7.0f);
	partCollider->SetShouldTriggerEvents(true);

	go->SetWorldPosition(264.0f, 241.0f);
	scene.Add(std::move(go));


	go = std::make_unique<dae::GameObject>();
	go->SetTag(make_sdbm_hash("BurgerPart"));
	go->AddComponent<dae::BurgerPartComponent>("BurgerParts/BurgerBunBottom.png", 8);
	partCollider = go->AddComponent<dae::RectCollider2DComponent>(32.0f, 7.0f);
	partCollider->SetShouldTriggerEvents(true);

	go->SetWorldPosition(264.0f, 278.0f);
	scene.Add(std::move(go));


	// Invisible walls
	go = std::make_unique<dae::GameObject>();
	partCollider = go->AddComponent<dae::RectCollider2DComponent>(4.0f, 100.0f);
	partCollider->SetShouldCollide(true);
	partCollider->SetIsStatic(true);

	go->SetWorldPosition(332.0f, 220.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	partCollider = go->AddComponent<dae::RectCollider2DComponent>(4.0f, 100.0f);
	partCollider->SetShouldCollide(true);
	partCollider->SetIsStatic(true);

	go->SetWorldPosition(164.0f, 220.0f);
	scene.Add(std::move(go));


	// First Enemy

	go = std::make_unique<dae::GameObject>();
	go->SetTag(make_sdbm_hash("Enemy"));

	spriteSheetComp = go->AddComponent<dae::SpritesheetComponent>("Enemies");
	spriteSheetComp->AddSprite("HotDogWalkingLeft.png", make_sdbm_hash("HotDogWalkingLeft"), dae::SpritesheetComponent::SpriteMetaData(2, 0, 0.12f));
	spriteSheetComp->AddSprite("HotDogWalkingRight.png", make_sdbm_hash("HotDogWalkingRight"), dae::SpritesheetComponent::SpriteMetaData(2, 0, 0.12f));
	spriteSheetComp->AddSprite("HotDogWalkingDown.png", make_sdbm_hash("HotDogWalkingDown"), dae::SpritesheetComponent::SpriteMetaData(2, 0, 0.12f));
	spriteSheetComp->AddSprite("HotDogWalkingUp.png", make_sdbm_hash("HotDogWalkingUp"), dae::SpritesheetComponent::SpriteMetaData(2, 0, 0.12f));
	spriteSheetComp->AddSprite("HotDogStunned.png", make_sdbm_hash("HotDogStunned"), dae::SpritesheetComponent::SpriteMetaData(2, 0, 0.2f));
	spriteSheetComp->AddSprite("HotDogDying.png", make_sdbm_hash("HotDogDying"), dae::SpritesheetComponent::SpriteMetaData(4, 0, 0.5f));
	spriteSheetComp->Play(make_sdbm_hash("HotDogWalkingLeft"));

	rectColliderComp = go->AddComponent<dae::RectCollider2DComponent>(16.0f, 16.0f);
	rectColliderComp->SetShouldTriggerEvents(true);

	go->AddComponent<dae::EnemyComponent>(EnemyType::HotDog);

	go->AddComponent<dae::EnemyMoveComponent>(playerPtr, 30.0f);
	go->SetWorldPosition(310, 265);

	//228 for top
	//265 for bot

	scene.Add(std::move(go));

	// Sound addition

	auto& sound = ServiceLocator::GetSoundSystem();

	sound.SetFolder("Sound");

	sound.LoadSound("BurgerStep.wav");
	sound.LoadSound("BurgerLand.wav");
	sound.LoadSound("BurgerFall.wav");
	sound.LoadSound("RoundClear.wav");
	sound.LoadSound("MainTheme.mp3", true);
	//sound.Play("MainTheme.mp3", 0.8f, true);

	gameHandlerComp->SwitchLevel("Menu");
}

int main(int, char* []) {
	find_resources();
	dae::Minigin engine{};
	engine.Run(load);
	return 0;
}