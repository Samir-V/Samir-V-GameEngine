#include <SDL.h>

#if _DEBUG
// ReSharper disable once CppUnusedIncludeDirective
#if __has_include(<vld.h>)
#include <vld.h>
#endif
#endif

#include "BurgerPartComponent.h"
#include "Minigin.h"

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

#include "GameHandlerComponent.h"
#include "HighScoreInputComponent.h"
#include "MenuCommand.h"
#include "MenuHUDComponent.h"
#include "MiscCommand.h"
#include "PepperDisplayComponent.h"
#include "ScoreComponent.h"
#include "HighScoreCommand.h"
#include "ScoreboardDisplayComponent.h"

void find_resources()
{
	constexpr int MAX_TRAVERSAL{ 3 };

	const std::filesystem::path resFolderName{ "Data" };
	int counter{ 0 };

	while (!exists(resFolderName) && counter < MAX_TRAVERSAL)
	{
		std::filesystem::current_path("..");
		counter++;
	}

	current_path(resFolderName);
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
			const auto collider = go->AddComponent<dae::RectCollider2DComponent>(
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

	for (auto& goData : doc.at("gameObjects"))
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
	std::srand(static_cast<unsigned int>(std::time(nullptr)));

	ServiceLocator::RegisterSoundSystem(std::make_unique<SDLSoundSystem>());
	register_factories();

	// Registering inputs
	auto& input = dae::InputManager::GetInstance();
	input.AddInputMap(make_sdbm_hash("MenuMap"));
	input.AddInputMap(make_sdbm_hash("GameplayMap"));
	input.AddInputMap(make_sdbm_hash("HighScoreInputMap"));
	input.AddInputMap(make_sdbm_hash("HighScoreViewMap"));
	input.SetActiveInputMap(make_sdbm_hash("MenuMap"));

	// Adding the gamehandler to the dont destroy on load scene
	auto& sceneDontDestroy = dae::SceneManager::GetInstance().CreateScene("DontDestroyOnLoadScene", true);
	auto go = std::make_unique<dae::GameObject>();
	const auto gameHandlerComp = go->AddComponent<dae::GameHandlerComponent>();
	gameHandlerComp->AddEnemySpawnPattern(dae::GameHandlerComponent::EnemySpawnPattern{ 3, 0, 1 });
	gameHandlerComp->AddEnemySpawnPattern(dae::GameHandlerComponent::EnemySpawnPattern{ 2, 1, 1 });
	gameHandlerComp->AddEnemySpawnPattern(dae::GameHandlerComponent::EnemySpawnPattern{ 2, 2, 1 });
	go->SetTag(make_sdbm_hash("GameHandler"));
	auto gameHandler = go.get();
	sceneDontDestroy.Add(std::move(go));

	// Adding misc commands
	input.RegisterKeyboardCommand(make_sdbm_hash("GameplayMap"), std::make_unique<dae::SkipLevel>(gameHandler), SDL_SCANCODE_F1, dae::InputManager::ActivationType::Pressing);
	input.RegisterKeyboardCommand(make_sdbm_hash("GameplayMap"), std::make_unique<dae::MuteSound>(gameHandler), SDL_SCANCODE_F2, dae::InputManager::ActivationType::Pressing);

	// Registering the non-playing scenes
	auto& sceneManager = dae::SceneManager::GetInstance();

	sceneManager.RegisterSceneFactory("Menu", [&sceneManager]() -> std::shared_ptr<dae::Scene>
		{
			// Menu scene
			auto fontBig = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 32);
			auto font = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 14);
			auto fontSmall = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 12);

			auto& input = dae::InputManager::GetInstance();

			auto menuScene = sceneManager.CreateSceneForFactory("Menu");
			auto go = std::make_unique<dae::GameObject>();
			auto textComp = go->AddComponent<dae::TextComponent>("Burger Time!", fontBig);
			textComp->SetLocalPosition(30, 40);
			menuScene->Add(std::move(go));

			go = std::make_unique<dae::GameObject>();
			auto textComp1 = go->AddComponent<dae::TextComponent>("Solo mode", font);
			textComp1->SetLocalPosition(85, 110);
			menuScene->Add(std::move(go));

			go = std::make_unique<dae::GameObject>();
			auto textComp2 = go->AddComponent<dae::TextComponent>("Co-op mode", font);
			textComp2->SetLocalPosition(85, 160);
			menuScene->Add(std::move(go));

			go = std::make_unique<dae::GameObject>();
			textComp = go->AddComponent<dae::TextComponent>("--->", font);
			textComp->SetLocalPosition(40, 110);

			go->AddComponent<dae::MenuHUDComponent>(textComp, std::vector{{textComp1, textComp2}});

			const auto sceneDontDestroy = dae::SceneManager::GetInstance().GetDontDestroyOnLoadScene();
			auto gameHandlerObject = sceneDontDestroy->GetGameObjectsWithTag(make_sdbm_hash("GameHandler")).front();

			input.RegisterControllerCommand(make_sdbm_hash("MenuMap"), std::make_unique<dae::AlterHUDCounter>(go.get(), -1), XINPUT_GAMEPAD_DPAD_UP, dae::InputManager::ActivationType::Pressing, 0);
			input.RegisterControllerCommand(make_sdbm_hash("MenuMap"), std::make_unique<dae::AlterHUDCounter>(go.get(), 1), XINPUT_GAMEPAD_DPAD_DOWN, dae::InputManager::ActivationType::Pressing, 0);
			input.RegisterControllerCommand(make_sdbm_hash("MenuMap"), std::make_unique<dae::ConfirmModeChoice>(go.get()), XINPUT_GAMEPAD_A, dae::InputManager::ActivationType::Pressing, 0);
			input.RegisterControllerCommand(make_sdbm_hash("MenuMap"), std::make_unique<dae::VisitHighScoreView>(gameHandlerObject), XINPUT_GAMEPAD_DPAD_RIGHT, dae::InputManager::ActivationType::Pressing, 0);

			input.RegisterKeyboardCommand(make_sdbm_hash("MenuMap"), std::make_unique<dae::AlterHUDCounter>(go.get(), -1), SDL_SCANCODE_UP, dae::InputManager::ActivationType::Pressing);
			input.RegisterKeyboardCommand(make_sdbm_hash("MenuMap"), std::make_unique<dae::AlterHUDCounter>(go.get(), 1), SDL_SCANCODE_DOWN, dae::InputManager::ActivationType::Pressing);
			input.RegisterKeyboardCommand(make_sdbm_hash("MenuMap"), std::make_unique<dae::ConfirmModeChoice>(go.get()), SDL_SCANCODE_SPACE, dae::InputManager::ActivationType::Pressing);
			input.RegisterKeyboardCommand(make_sdbm_hash("MenuMap"), std::make_unique<dae::VisitHighScoreView>(gameHandlerObject), SDL_SCANCODE_RIGHT, dae::InputManager::ActivationType::Pressing);

			menuScene->Add(std::move(go));

			go = std::make_unique<dae::GameObject>();
			const auto controlsPanel = go->AddComponent<dae::TextComponent>("To go to Scoreboard", fontSmall);
			controlsPanel->SetLocalPosition(0, 213);
			menuScene->Add(std::move(go));

			go = std::make_unique<dae::GameObject>();
			const auto controlsPanel2 = go->AddComponent<dae::TextComponent>("Press Right on D-PAD or Keyboard", fontSmall);
			controlsPanel2->SetLocalPosition(0, 227);
			menuScene->Add(std::move(go));

			return menuScene;
		});

	sceneManager.RegisterSceneFactory("HighScoreInput", [&sceneManager]() -> std::shared_ptr<dae::Scene>
		{
			// HighScore scene
			auto fontBig = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 32);
			auto font = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 14);
			auto fontSmall = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 12);

			auto& input = dae::InputManager::GetInstance();

			auto highScoreInputScene = sceneManager.CreateSceneForFactory("HighScoreInput");
			
			auto go = std::make_unique<dae::GameObject>();
			auto scoreShowcase = go->AddComponent<dae::TextComponent>("Your score: 0", fontBig);
			scoreShowcase->SetLocalPosition(30, 40);
			highScoreInputScene->Add(std::move(go));

			go = std::make_unique<dae::GameObject>();
			auto nameShowcase = go->AddComponent<dae::TextComponent>("Your name", fontBig);
			nameShowcase->SetLocalPosition(30, 80);
			highScoreInputScene->Add(std::move(go));

			go = std::make_unique<dae::GameObject>();
			const auto leftArrow = go->AddComponent<dae::TextComponent>("<---", font);
			leftArrow->SetLocalPosition(63, 130);
			highScoreInputScene->Add(std::move(go));

			go = std::make_unique<dae::GameObject>();
			const auto leftLetterShowcase = go->AddComponent<dae::TextComponent>("", font);
			leftLetterShowcase->SetLocalPosition(90, 130);
			highScoreInputScene->Add(std::move(go));

			go = std::make_unique<dae::GameObject>();
			const auto mainLetterShowcase = go->AddComponent<dae::TextComponent>("", fontBig);
			mainLetterShowcase->SetLocalPosition(110, 120);
			highScoreInputScene->Add(std::move(go));

			go = std::make_unique<dae::GameObject>();
			const auto rightLetterShowcase = go->AddComponent<dae::TextComponent>("", font);
			rightLetterShowcase->SetLocalPosition(140, 130);
			highScoreInputScene->Add(std::move(go));

			go = std::make_unique<dae::GameObject>();
			const auto rightArrow = go->AddComponent<dae::TextComponent>("--->", font);
			rightArrow->SetLocalPosition(155, 130);
			highScoreInputScene->Add(std::move(go));

			go = std::make_unique<dae::GameObject>();
			go->SetTag(make_sdbm_hash("HighScoreInputShowcase"));
			auto highScoreInputObject = go.get();
			go->AddComponent<dae::HighScoreInputComponent>(scoreShowcase, nameShowcase, std::vector{ leftLetterShowcase, mainLetterShowcase, rightLetterShowcase });
			highScoreInputScene->Add(std::move(go));

			go = std::make_unique<dae::GameObject>();
			const auto controlsPanel = go->AddComponent<dae::TextComponent>("Use arrows/D-PAD to switch letters", fontSmall);
			controlsPanel->SetLocalPosition(0, 185);
			highScoreInputScene->Add(std::move(go));

			go = std::make_unique<dae::GameObject>();
			const auto controlsPanel2 = go->AddComponent<dae::TextComponent>("Press E/A to confirm a letter", fontSmall);
			controlsPanel2->SetLocalPosition(0, 199);
			highScoreInputScene->Add(std::move(go));

			go = std::make_unique<dae::GameObject>();
			const auto controlsPanel3 = go->AddComponent<dae::TextComponent>("Remove letters with Backspace/B", fontSmall);
			controlsPanel3->SetLocalPosition(0, 213);
			highScoreInputScene->Add(std::move(go));

			go = std::make_unique<dae::GameObject>();
			const auto controlsPanel4 = go->AddComponent<dae::TextComponent>("Confirm name with Space/X", fontSmall);
			controlsPanel4->SetLocalPosition(0, 227);
			highScoreInputScene->Add(std::move(go));

			input.RegisterControllerCommand(make_sdbm_hash("HighScoreInputMap"), std::make_unique<dae::AlterLetterIndex>(highScoreInputObject, -1), XINPUT_GAMEPAD_DPAD_LEFT, dae::InputManager::ActivationType::Pressing, 0);
			input.RegisterControllerCommand(make_sdbm_hash("HighScoreInputMap"), std::make_unique<dae::AlterLetterIndex>(highScoreInputObject, 1), XINPUT_GAMEPAD_DPAD_RIGHT, dae::InputManager::ActivationType::Pressing, 0);

			input.RegisterControllerCommand(make_sdbm_hash("HighScoreInputMap"), std::make_unique<dae::ConfirmLetter>(highScoreInputObject), XINPUT_GAMEPAD_A, dae::InputManager::ActivationType::Pressing, 0);
			input.RegisterControllerCommand(make_sdbm_hash("HighScoreInputMap"), std::make_unique<dae::ConfirmName>(highScoreInputObject), XINPUT_GAMEPAD_X, dae::InputManager::ActivationType::Pressing, 0);

			input.RegisterControllerCommand(make_sdbm_hash("HighScoreInputMap"), std::make_unique<dae::RemoveLetter>(highScoreInputObject), XINPUT_GAMEPAD_B, dae::InputManager::ActivationType::Pressing, 0);


			input.RegisterKeyboardCommand(make_sdbm_hash("HighScoreInputMap"), std::make_unique<dae::AlterLetterIndex>(highScoreInputObject, -1), SDL_SCANCODE_LEFT, dae::InputManager::ActivationType::Pressing);
			input.RegisterKeyboardCommand(make_sdbm_hash("HighScoreInputMap"), std::make_unique<dae::AlterLetterIndex>(highScoreInputObject, 1), SDL_SCANCODE_RIGHT, dae::InputManager::ActivationType::Pressing);

			input.RegisterKeyboardCommand(make_sdbm_hash("HighScoreInputMap"), std::make_unique<dae::ConfirmLetter>(highScoreInputObject), SDL_SCANCODE_E, dae::InputManager::ActivationType::Pressing);
			input.RegisterKeyboardCommand(make_sdbm_hash("HighScoreInputMap"), std::make_unique<dae::ConfirmName>(highScoreInputObject), SDL_SCANCODE_SPACE, dae::InputManager::ActivationType::Pressing);

			input.RegisterKeyboardCommand(make_sdbm_hash("HighScoreInputMap"), std::make_unique<dae::RemoveLetter>(highScoreInputObject), SDL_SCANCODE_BACKSPACE, dae::InputManager::ActivationType::Pressing);

			return highScoreInputScene;
		});

	sceneManager.RegisterSceneFactory("HighScoreView", [&sceneManager]() -> std::shared_ptr<dae::Scene>
		{
			// HighScore view scene
			auto fontBig = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 32);
			auto font = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 14);
			auto fontSmall = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 12);

			auto& input = dae::InputManager::GetInstance();

			auto highScoreViewScene = sceneManager.CreateSceneForFactory("HighScoreView");

			auto go = std::make_unique<dae::GameObject>();
			const auto title = go->AddComponent<dae::TextComponent>("Scoreboard", fontBig);
			title->SetLocalPosition(30, 40);
			highScoreViewScene->Add(std::move(go));

			go = std::make_unique<dae::GameObject>();
			const auto firstPlayer = go->AddComponent<dae::TextComponent>("Empty", font);
			firstPlayer->SetLocalPosition(30, 80);
			highScoreViewScene->Add(std::move(go));

			go = std::make_unique<dae::GameObject>();
			const auto secondPlayer = go->AddComponent<dae::TextComponent>("Empty", font);
			secondPlayer->SetLocalPosition(30, 100);
			highScoreViewScene->Add(std::move(go));

			go = std::make_unique<dae::GameObject>();
			const auto thirdPlayer = go->AddComponent<dae::TextComponent>("Empty", font);
			thirdPlayer->SetLocalPosition(30, 120);
			highScoreViewScene->Add(std::move(go));

			go = std::make_unique<dae::GameObject>();
			const auto fourthPlayer = go->AddComponent<dae::TextComponent>("Empty", font);
			fourthPlayer->SetLocalPosition(30, 140);
			highScoreViewScene->Add(std::move(go));

			go = std::make_unique<dae::GameObject>();
			const auto fifthPlayer = go->AddComponent<dae::TextComponent>("Empty", font);
			fifthPlayer->SetLocalPosition(30, 160);
			highScoreViewScene->Add(std::move(go));

			go = std::make_unique<dae::GameObject>();
			go->AddComponent<dae::ScoreboardDisplayComponent>(std::vector{ firstPlayer, secondPlayer, thirdPlayer, fourthPlayer, fifthPlayer });
			highScoreViewScene->Add(std::move(go));

			const auto sceneDontDestroy = dae::SceneManager::GetInstance().GetDontDestroyOnLoadScene();
			auto gameHandlerObject = sceneDontDestroy->GetGameObjectsWithTag(make_sdbm_hash("GameHandler")).front();

			input.RegisterControllerCommand(make_sdbm_hash("HighScoreViewMap"), std::make_unique<dae::ReturnToMenu>(gameHandlerObject), XINPUT_GAMEPAD_DPAD_LEFT, dae::InputManager::ActivationType::Pressing, 0);
			input.RegisterKeyboardCommand(make_sdbm_hash("HighScoreViewMap"), std::make_unique<dae::ReturnToMenu>(gameHandlerObject), SDL_SCANCODE_LEFT, dae::InputManager::ActivationType::Pressing);

			go = std::make_unique<dae::GameObject>();
			const auto controlsPanel = go->AddComponent<dae::TextComponent>("To go to Main Menu", fontSmall);
			controlsPanel->SetLocalPosition(0, 213);
			highScoreViewScene->Add(std::move(go));

			go = std::make_unique<dae::GameObject>();
			const auto controlsPanel2 = go->AddComponent<dae::TextComponent>("Press Left on D-PAD or Keyboard", fontSmall);
			controlsPanel2->SetLocalPosition(0, 227);
			highScoreViewScene->Add(std::move(go));

			return highScoreViewScene;
		});

	// Registering the rest of dont destroy on load scene
	// Peter Pepper

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

	const auto rectColliderCompController = go->AddComponent<dae::RectCollider2DComponent>(16.0f, 16.0f);
	rectColliderCompController->SetShouldTriggerEvents(true);

	go->AddComponent<dae::MoveComponent>(50.0f);
	go->AddComponent<dae::PeterPepperComponent>();

	input.RegisterControllerCommand(make_sdbm_hash("GameplayMap"), std::make_unique<MoveInDirection>(go.get(), glm::vec2(0.0f, -1.0f)), XINPUT_GAMEPAD_DPAD_UP, dae::InputManager::ActivationType::Holding, 0);
	input.RegisterControllerCommand(make_sdbm_hash("GameplayMap"), std::make_unique<MoveInDirection>(go.get(), glm::vec2(-1.0f, 0.0f)), XINPUT_GAMEPAD_DPAD_LEFT, dae::InputManager::ActivationType::Holding, 0);
	input.RegisterControllerCommand(make_sdbm_hash("GameplayMap"), std::make_unique<MoveInDirection>(go.get(), glm::vec2(0.0f, 1.0f)), XINPUT_GAMEPAD_DPAD_DOWN, dae::InputManager::ActivationType::Holding, 0);
	input.RegisterControllerCommand(make_sdbm_hash("GameplayMap"), std::make_unique<MoveInDirection>(go.get(), glm::vec2(1.0f, 0.0f)), XINPUT_GAMEPAD_DPAD_RIGHT, dae::InputManager::ActivationType::Holding, 0);

	input.RegisterControllerCommand(make_sdbm_hash("GameplayMap"), std::make_unique<SprayPepper>(go.get()), XINPUT_GAMEPAD_X, dae::InputManager::ActivationType::Pressing, 0);

	input.RegisterKeyboardCommand(make_sdbm_hash("GameplayMap"), std::make_unique<MoveInDirection>(go.get(), glm::vec2(0.0f, -1.0f)), SDL_SCANCODE_W, dae::InputManager::ActivationType::Holding);
	input.RegisterKeyboardCommand(make_sdbm_hash("GameplayMap"), std::make_unique<MoveInDirection>(go.get(), glm::vec2(-1.0f, 0.0f)), SDL_SCANCODE_A, dae::InputManager::ActivationType::Holding);
	input.RegisterKeyboardCommand(make_sdbm_hash("GameplayMap"), std::make_unique<MoveInDirection>(go.get(), glm::vec2(0.0f, 1.0f)), SDL_SCANCODE_S, dae::InputManager::ActivationType::Holding);
	input.RegisterKeyboardCommand(make_sdbm_hash("GameplayMap"), std::make_unique<MoveInDirection>(go.get(), glm::vec2(1.0f, 0.0f)), SDL_SCANCODE_D, dae::InputManager::ActivationType::Holding);

	input.RegisterKeyboardCommand(make_sdbm_hash("GameplayMap"), std::make_unique<SprayPepper>(go.get()), SDL_SCANCODE_K, dae::InputManager::ActivationType::Pressing);

	go->SetIsActive(false);

	dae::GameObject* playerPtr = go.get();

	sceneDontDestroy.Add(std::move(go));

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

	const auto rectColliderComp = go->AddComponent<dae::RectCollider2DComponent>(16.0f, 16.0f);
	rectColliderComp->SetShouldTriggerEvents(true);

	go->SetIsActive(false);

	sceneDontDestroy.Add(std::move(go));

	// Gameplay HUD

	auto fontBig = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 32);
	auto font = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 14);

	auto goHUD = std::make_unique<dae::GameObject>();
	goHUD->SetTag(make_sdbm_hash("GameplayHUD"));
	const auto goHUDptr = goHUD.get();
	sceneDontDestroy.Add(std::move(goHUD));

	go = std::make_unique<dae::GameObject>();
	go->SetParent(goHUDptr, false);
	auto textComp = go->AddComponent<dae::TextComponent>("Score: 0", font);
	textComp->SetLocalPosition(5, 0);
	sceneDontDestroy.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->SetParent(goHUDptr, false);
	go->SetTag(make_sdbm_hash("GameplayHUDScore"));
	go->AddComponent<dae::ScoreComponent>(textComp);
	sceneDontDestroy.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->SetParent(goHUDptr, false);
	go->SetLocalPosition(0, 220.0f);
	const auto firstLifeDisplay = go->AddComponent<dae::Texture2DComponent>("HUD/Life.png");
	const auto secondLifeDisplay = go->AddComponent<dae::Texture2DComponent>("HUD/Life.png");
	firstLifeDisplay->SetLocalPosition(0, 0);
	secondLifeDisplay->SetLocalPosition(0, -10);
	go->AddComponent<dae::HealthDisplayComponent>(std::vector{ firstLifeDisplay, secondLifeDisplay }, playerPtr);
	sceneDontDestroy.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->SetParent(goHUDptr, false);
	textComp = go->AddComponent<dae::TextComponent>("Peppers: 5", font);
	textComp->SetLocalPosition(170, 0);
	sceneDontDestroy.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->SetParent(goHUDptr, false);
	go->AddComponent<dae::PepperDisplayComponent>(textComp, playerPtr);
	sceneDontDestroy.Add(std::move(go));

	// Registering playable levels

	sceneManager.RegisterSceneFactory("Level1", [&sceneManager]() -> std::shared_ptr<dae::Scene>
		{
			auto scene = sceneManager.CreateSceneForFactory("Level1");
			load_level_from_json("level1.json", *scene);
			return scene;
		});

	sceneManager.RegisterSceneFactory("Level2", [&sceneManager]() -> std::shared_ptr<dae::Scene>
		{
			auto scene = sceneManager.CreateSceneForFactory("Level2");
			load_level_from_json("level2.json", *scene);
			return scene;
		});

	sceneManager.RegisterSceneFactory("Level3", [&sceneManager]() -> std::shared_ptr<dae::Scene>
		{
			auto scene = sceneManager.CreateSceneForFactory("Level3");
			load_level_from_json("level3.json", *scene);
			return scene;
		});

	// Sound addition

	auto& sound = ServiceLocator::GetSoundSystem();

	sound.SetFolder("Sound");

	sound.LoadSound("BurgerStep.wav");
	sound.LoadSound("BurgerLand.wav");
	sound.LoadSound("BurgerFall.wav");
	sound.LoadSound("RoundClear.wav");
	sound.LoadSound("EnemyFall.wav");
	sound.LoadSound("EnemySprayed.wav");
	sound.LoadSound("Death.wav");
	sound.LoadSound("EnemySquashed.wav");
	sound.LoadSound("PepperShake.wav");
	sound.LoadSound("MainTheme.mp3", true);

	gameHandlerComp->SwitchLevel("Menu");
}

int main(int, char* []) {
	find_resources();
	dae::Minigin engine{};
	engine.Run(load);
	return 0;
}