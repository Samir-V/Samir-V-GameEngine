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
	gameHandlerComp->AddEnemySpawnPattern(dae::GameHandlerComponent::EnemySpawnPattern{ 3, 0, 1 });
	gameHandlerComp->AddEnemySpawnPattern(dae::GameHandlerComponent::EnemySpawnPattern{ 3, 0, 1 });
	gameHandlerComp->AddEnemySpawnPattern(dae::GameHandlerComponent::EnemySpawnPattern{ 2, 2, 1 });
	go->SetTag(make_sdbm_hash("GameHandler"));
	sceneDontDestroy.Add(std::move(go));

	// Menu scene
	auto& menuScene = dae::SceneManager::GetInstance().CreateScene("Menu", false);
	auto fontBig = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 32);
	auto font = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 14);

	go = std::make_unique<dae::GameObject>();

	auto textComp = go->AddComponent<dae::TextComponent>("Burger Time!", fontBig);
	textComp->SetLocalPosition(30, 40);

	auto textComp1 = go->AddComponent<dae::TextComponent>("Solo mode", font);
	textComp1->SetLocalPosition(85, 110);

	auto textComp2 = go->AddComponent<dae::TextComponent>("Co-op mode", font);
	textComp2->SetLocalPosition(85, 160);

	auto textComp3 = go->AddComponent<dae::TextComponent>("Versus mode", font);
	textComp3->SetLocalPosition(85, 210);


	textComp = go->AddComponent<dae::TextComponent>("--->", font);
	textComp->SetLocalPosition(40, 110);

	go->AddComponent<dae::MenuHUDComponent>(textComp, std::vector<dae::TextComponent*>{{textComp1, textComp2, textComp3}});

	input.RegisterControllerCommand(make_sdbm_hash("MenuMap"), std::make_unique<dae::AlterHUDCounter>(go.get(), -1), XINPUT_GAMEPAD_DPAD_UP, dae::InputManager::ActivationType::Pressing, 0);
	input.RegisterControllerCommand(make_sdbm_hash("MenuMap"), std::make_unique<dae::AlterHUDCounter>(go.get(), 1), XINPUT_GAMEPAD_DPAD_DOWN, dae::InputManager::ActivationType::Pressing, 0);
	input.RegisterControllerCommand(make_sdbm_hash("MenuMap"), std::make_unique<dae::ConfirmModeChoice>(go.get()), XINPUT_GAMEPAD_A, dae::InputManager::ActivationType::Pressing, 0);

	menuScene.Add(std::move(go));

	// Level 1
	auto& scene = dae::SceneManager::GetInstance().CreateScene("Level1", false);

	/*register_factories();
	load_level_from_json("levels.json", scene);*/

	go = std::make_unique<dae::GameObject>();
	textComp = go->AddComponent<dae::TextComponent>("Score: 0", font);
	textComp->SetLocalPosition(5, 0);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::ScoreComponent>(textComp);
	scene.Add(std::move(go));

	// Column x = 16
	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/ShortPlatform.png");
	go->AddComponent<dae::RectCollider2DComponent>(16.0f, 4.0f);
	go->SetTag(make_sdbm_hash("Platform"));
	go->SetWorldPosition(16.0f, 36.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/ShortPlatform.png");
	go->AddComponent<dae::RectCollider2DComponent>(16.0f, 4.0f);
	go->SetTag(make_sdbm_hash("Platform"));
	go->SetWorldPosition(16.0f, 72.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/ShortPlatform.png");
	go->AddComponent<dae::RectCollider2DComponent>(16.0f, 4.0f);
	go->SetTag(make_sdbm_hash("Platform"));
	go->SetWorldPosition(16.0f, 108.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/ShortPlatform.png");
	go->AddComponent<dae::RectCollider2DComponent>(16.0f, 4.0f);
	go->SetTag(make_sdbm_hash("Platform"));
	go->SetWorldPosition(16.0f, 160.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/ShortPlatform.png");
	go->AddComponent<dae::RectCollider2DComponent>(16.0f, 4.0f);
	go->SetTag(make_sdbm_hash("Platform"));
	go->SetWorldPosition(16.0f, 196.0f);
	scene.Add(std::move(go));

	// Column x = 19
	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/Ladder.png");
	go->AddComponent<dae::RectCollider2DComponent>(10.0f, 16.0f);
	go->SetTag(make_sdbm_hash("Ladder"));
	go->SetWorldPosition(19.0f, 40.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/Ladder.png");
	go->AddComponent<dae::RectCollider2DComponent>(10.0f, 16.0f);
	go->SetTag(make_sdbm_hash("Ladder"));
	go->SetWorldPosition(19.0f, 56.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/Ladder.png");
	go->AddComponent<dae::RectCollider2DComponent>(10.0f, 16.0f);
	go->SetTag(make_sdbm_hash("Ladder"));
	go->SetWorldPosition(19.0f, 112.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/Ladder.png");
	go->AddComponent<dae::RectCollider2DComponent>(10.0f, 16.0f);
	go->SetTag(make_sdbm_hash("Ladder"));
	go->SetWorldPosition(19.0f, 128.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/Ladder.png");
	go->AddComponent<dae::RectCollider2DComponent>(10.0f, 16.0f);
	go->SetTag(make_sdbm_hash("Ladder"));
	go->SetWorldPosition(19.0f, 144.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/Ladder.png");
	go->AddComponent<dae::RectCollider2DComponent>(10.0f, 16.0f);
	go->SetTag(make_sdbm_hash("Ladder"));
	go->SetWorldPosition(19.0f, 164.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/Ladder.png");
	go->AddComponent<dae::RectCollider2DComponent>(10.0f, 16.0f);
	go->SetTag(make_sdbm_hash("Ladder"));
	go->SetWorldPosition(19.0f, 180.0f);
	scene.Add(std::move(go));

	// Column x = 32
	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/WidePlatform.png");
	go->AddComponent<dae::RectCollider2DComponent>(32.0f, 4.0f);
	go->SetTag(make_sdbm_hash("Platform"));
	go->SetWorldPosition(32.0f, 36.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/WidePlatform.png");
	go->AddComponent<dae::RectCollider2DComponent>(32.0f, 4.0f);
	go->SetTag(make_sdbm_hash("Platform"));
	go->SetWorldPosition(32.0f, 72.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/WidePlatform.png");
	go->AddComponent<dae::RectCollider2DComponent>(32.0f, 4.0f);
	go->SetTag(make_sdbm_hash("Platform"));
	go->SetWorldPosition(32.0f, 108.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/WidePlatform.png");
	go->AddComponent<dae::RectCollider2DComponent>(32.0f, 4.0f);
	go->SetTag(make_sdbm_hash("Platform"));
	go->SetWorldPosition(32.0f, 160.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/WidePlatform.png");
	go->AddComponent<dae::RectCollider2DComponent>(32.0f, 4.0f);
	go->SetTag(make_sdbm_hash("Platform"));
	go->SetWorldPosition(32.0f, 196.0f);
	scene.Add(std::move(go));

	// Column x = 43
	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/Ladder.png");
	go->AddComponent<dae::RectCollider2DComponent>(10.0f, 16.0f);
	go->SetTag(make_sdbm_hash("Ladder"));
	go->SetWorldPosition(43.0f, 76.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/Ladder.png");
	go->AddComponent<dae::RectCollider2DComponent>(10.0f, 16.0f);
	go->SetTag(make_sdbm_hash("Ladder"));
	go->SetWorldPosition(43.0f, 92.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/Ladder.png");
	go->AddComponent<dae::RectCollider2DComponent>(10.0f, 16.0f);
	go->SetTag(make_sdbm_hash("Ladder"));
	go->SetWorldPosition(43.0f, 112.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/Ladder.png");
	go->AddComponent<dae::RectCollider2DComponent>(10.0f, 16.0f);
	go->SetTag(make_sdbm_hash("Ladder"));
	go->SetWorldPosition(43.0f, 128.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/Ladder.png");
	go->AddComponent<dae::RectCollider2DComponent>(10.0f, 16.0f);
	go->SetTag(make_sdbm_hash("Ladder"));
	go->SetWorldPosition(43.0f, 144.0f);
	scene.Add(std::move(go));

	// Column x = 64
	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/ShortPlatform.png");
	go->AddComponent<dae::RectCollider2DComponent>(16.0f, 4.0f);
	go->SetTag(make_sdbm_hash("Platform"));
	go->SetWorldPosition(64.0f, 36.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/ShortPlatform.png");
	go->AddComponent<dae::RectCollider2DComponent>(16.0f, 4.0f);
	go->SetTag(make_sdbm_hash("Platform"));
	go->SetWorldPosition(64.0f, 72.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/ShortPlatform.png");
	go->AddComponent<dae::RectCollider2DComponent>(16.0f, 4.0f);
	go->SetTag(make_sdbm_hash("Platform"));
	go->SetWorldPosition(64.0f, 88.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/ShortPlatform.png");
	go->AddComponent<dae::RectCollider2DComponent>(16.0f, 4.0f);
	go->SetTag(make_sdbm_hash("Platform"));
	go->SetWorldPosition(64.0f, 108.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/ShortPlatform.png");
	go->AddComponent<dae::RectCollider2DComponent>(16.0f, 4.0f);
	go->SetTag(make_sdbm_hash("Platform"));
	go->SetWorldPosition(64.0f, 124.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/ShortPlatform.png");
	go->AddComponent<dae::RectCollider2DComponent>(16.0f, 4.0f);
	go->SetTag(make_sdbm_hash("Platform"));
	go->SetWorldPosition(64.0f, 160.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/ShortPlatform.png");
	go->AddComponent<dae::RectCollider2DComponent>(16.0f, 4.0f);
	go->SetTag(make_sdbm_hash("Platform"));
	go->SetWorldPosition(64.0f, 196.0f);
	scene.Add(std::move(go));

	// Column x = 67
	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/Ladder.png");
	go->AddComponent<dae::RectCollider2DComponent>(10.0f, 16.0f);
	go->SetTag(make_sdbm_hash("Ladder"));
	go->SetWorldPosition(67.0f, 40.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/Ladder.png");
	go->AddComponent<dae::RectCollider2DComponent>(10.0f, 16.0f);
	go->SetTag(make_sdbm_hash("Ladder"));
	go->SetWorldPosition(67.0f, 56.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/ShortLadder.png");
	go->AddComponent<dae::RectCollider2DComponent>(10.0f, 12.0f);
	go->SetTag(make_sdbm_hash("Ladder"));
	go->SetWorldPosition(67.0f, 76.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/Ladder.png");
	go->AddComponent<dae::RectCollider2DComponent>(10.0f, 16.0f);
	go->SetTag(make_sdbm_hash("Ladder"));
	go->SetWorldPosition(67.0f, 92.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/ShortLadder.png");
	go->AddComponent<dae::RectCollider2DComponent>(10.0f, 12.0f);
	go->SetTag(make_sdbm_hash("Ladder"));
	go->SetWorldPosition(67.0f, 112.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/Ladder.png");
	go->AddComponent<dae::RectCollider2DComponent>(10.0f, 16.0f);
	go->SetTag(make_sdbm_hash("Ladder"));
	go->SetWorldPosition(67.0f, 128.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/Ladder.png");
	go->AddComponent<dae::RectCollider2DComponent>(10.0f, 16.0f);
	go->SetTag(make_sdbm_hash("Ladder"));
	go->SetWorldPosition(67.0f, 144.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/Ladder.png");
	go->AddComponent<dae::RectCollider2DComponent>(10.0f, 16.0f);
	go->SetTag(make_sdbm_hash("Ladder"));
	go->SetWorldPosition(67.0f, 164.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/Ladder.png");
	go->AddComponent<dae::RectCollider2DComponent>(10.0f, 16.0f);
	go->SetTag(make_sdbm_hash("Ladder"));
	go->SetWorldPosition(67.0f, 180.0f);
	scene.Add(std::move(go));

	// Column x = 80
	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/WidePlatform.png");
	go->AddComponent<dae::RectCollider2DComponent>(32.0f, 4.0f);
	go->SetTag(make_sdbm_hash("Platform"));
	go->SetWorldPosition(80.0f, 36.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/WidePlatform.png");
	go->AddComponent<dae::RectCollider2DComponent>(32.0f, 4.0f);
	go->SetTag(make_sdbm_hash("Platform"));
	go->SetWorldPosition(80.0f, 88.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/WidePlatform.png");
	go->AddComponent<dae::RectCollider2DComponent>(32.0f, 4.0f);
	go->SetTag(make_sdbm_hash("Platform"));
	go->SetWorldPosition(80.0f, 124.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/WidePlatform.png");
	go->AddComponent<dae::RectCollider2DComponent>(32.0f, 4.0f);
	go->SetTag(make_sdbm_hash("Platform"));
	go->SetWorldPosition(80.0f, 160.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/WidePlatform.png");
	go->AddComponent<dae::RectCollider2DComponent>(32.0f, 4.0f);
	go->SetTag(make_sdbm_hash("Platform"));
	go->SetWorldPosition(80.0f, 196.0f);
	scene.Add(std::move(go));

	// Column x = 91
	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/Ladder.png");
	go->AddComponent<dae::RectCollider2DComponent>(10.0f, 16.0f);
	go->SetTag(make_sdbm_hash("Ladder"));
	go->SetWorldPosition(91.0f, 40.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/Ladder.png");
	go->AddComponent<dae::RectCollider2DComponent>(10.0f, 16.0f);
	go->SetTag(make_sdbm_hash("Ladder"));
	go->SetWorldPosition(91.0f, 56.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/Ladder.png");
	go->AddComponent<dae::RectCollider2DComponent>(10.0f, 16.0f);
	go->SetTag(make_sdbm_hash("Ladder"));
	go->SetWorldPosition(91.0f, 72.0f);
	scene.Add(std::move(go));

	// Column x = 112
	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/ShortPlatform.png");
	go->AddComponent<dae::RectCollider2DComponent>(16.0f, 4.0f);
	go->SetTag(make_sdbm_hash("Platform"));
	go->SetWorldPosition(112.0f, 36.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/ShortPlatform.png");
	go->AddComponent<dae::RectCollider2DComponent>(16.0f, 4.0f);
	go->SetTag(make_sdbm_hash("Platform"));
	go->SetWorldPosition(112.0f, 72.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/ShortPlatform.png");
	go->AddComponent<dae::RectCollider2DComponent>(16.0f, 4.0f);
	go->SetTag(make_sdbm_hash("Platform"));
	go->SetWorldPosition(112.0f, 88.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/ShortPlatform.png");
	go->AddComponent<dae::RectCollider2DComponent>(16.0f, 4.0f);
	go->SetTag(make_sdbm_hash("Platform"));
	go->SetWorldPosition(112.0f, 124.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/ShortPlatform.png");
	go->AddComponent<dae::RectCollider2DComponent>(16.0f, 4.0f);
	go->SetTag(make_sdbm_hash("Platform"));
	go->SetWorldPosition(112.0f, 160.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/ShortPlatform.png");
	go->AddComponent<dae::RectCollider2DComponent>(16.0f, 4.0f);
	go->SetTag(make_sdbm_hash("Platform"));
	go->SetWorldPosition(112.0f, 196.0f);
	scene.Add(std::move(go));

	// Column x = 115
	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/Ladder.png");
	go->AddComponent<dae::RectCollider2DComponent>(10.0f, 16.0f);
	go->SetTag(make_sdbm_hash("Ladder"));
	go->SetWorldPosition(115.0f, 40.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/Ladder.png");
	go->AddComponent<dae::RectCollider2DComponent>(10.0f, 16.0f);
	go->SetTag(make_sdbm_hash("Ladder"));
	go->SetWorldPosition(115.0f, 56.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/ShortLadder.png");
	go->AddComponent<dae::RectCollider2DComponent>(10.0f, 12.0f);
	go->SetTag(make_sdbm_hash("Ladder"));
	go->SetWorldPosition(115.0f, 76.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/Ladder.png");
	go->AddComponent<dae::RectCollider2DComponent>(10.0f, 16.0f);
	go->SetTag(make_sdbm_hash("Ladder"));
	go->SetWorldPosition(115.0f, 92.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/Ladder.png");
	go->AddComponent<dae::RectCollider2DComponent>(10.0f, 16.0f);
	go->SetTag(make_sdbm_hash("Ladder"));
	go->SetWorldPosition(115.0f, 108.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/Ladder.png");
	go->AddComponent<dae::RectCollider2DComponent>(10.0f, 16.0f);
	go->SetTag(make_sdbm_hash("Ladder"));
	go->SetWorldPosition(115.0f, 128.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/Ladder.png");
	go->AddComponent<dae::RectCollider2DComponent>(10.0f, 16.0f);
	go->SetTag(make_sdbm_hash("Ladder"));
	go->SetWorldPosition(115.0f, 144.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/Ladder.png");
	go->AddComponent<dae::RectCollider2DComponent>(10.0f, 16.0f);
	go->SetTag(make_sdbm_hash("Ladder"));
	go->SetWorldPosition(115.0f, 164.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/Ladder.png");
	go->AddComponent<dae::RectCollider2DComponent>(10.0f, 16.0f);
	go->SetTag(make_sdbm_hash("Ladder"));
	go->SetWorldPosition(115.0f, 180.0f);
	scene.Add(std::move(go));

	// Column x = 128
	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/WidePlatform.png");
	go->AddComponent<dae::RectCollider2DComponent>(32.0f, 4.0f);
	go->SetTag(make_sdbm_hash("Platform"));
	go->SetWorldPosition(128.0f, 36.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/WidePlatform.png");
	go->AddComponent<dae::RectCollider2DComponent>(32.0f, 4.0f);
	go->SetTag(make_sdbm_hash("Platform"));
	go->SetWorldPosition(128.0f, 72.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/WidePlatform.png");
	go->AddComponent<dae::RectCollider2DComponent>(32.0f, 4.0f);
	go->SetTag(make_sdbm_hash("Platform"));
	go->SetWorldPosition(128.0f, 124.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/WidePlatform.png");
	go->AddComponent<dae::RectCollider2DComponent>(32.0f, 4.0f);
	go->SetTag(make_sdbm_hash("Platform"));
	go->SetWorldPosition(128.0f, 160.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/WidePlatform.png");
	go->AddComponent<dae::RectCollider2DComponent>(32.0f, 4.0f);
	go->SetTag(make_sdbm_hash("Platform"));
	go->SetWorldPosition(128.0f, 196.0f);
	scene.Add(std::move(go));

	// Column x = 139
	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/Ladder.png");
	go->AddComponent<dae::RectCollider2DComponent>(10.0f, 16.0f);
	go->SetTag(make_sdbm_hash("Ladder"));
	go->SetWorldPosition(139, 76.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/Ladder.png");
	go->AddComponent<dae::RectCollider2DComponent>(10.0f, 16.0f);
	go->SetTag(make_sdbm_hash("Ladder"));
	go->SetWorldPosition(139, 92.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/Ladder.png");
	go->AddComponent<dae::RectCollider2DComponent>(10.0f, 16.0f);
	go->SetTag(make_sdbm_hash("Ladder"));
	go->SetWorldPosition(139, 108.0f);
	scene.Add(std::move(go));

	// Column x = 160
	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/ShortPlatform.png");
	go->AddComponent<dae::RectCollider2DComponent>(16.0f, 4.0f);
	go->SetTag(make_sdbm_hash("Platform"));
	go->SetWorldPosition(160.0f, 36.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/ShortPlatform.png");
	go->AddComponent<dae::RectCollider2DComponent>(16.0f, 4.0f);
	go->SetTag(make_sdbm_hash("Platform"));
	go->SetWorldPosition(160.0f, 72.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/ShortPlatform.png");
	go->AddComponent<dae::RectCollider2DComponent>(16.0f, 4.0f);
	go->SetTag(make_sdbm_hash("Platform"));
	go->SetWorldPosition(160.0f, 108.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/ShortPlatform.png");
	go->AddComponent<dae::RectCollider2DComponent>(16.0f, 4.0f);
	go->SetTag(make_sdbm_hash("Platform"));
	go->SetWorldPosition(160.0f, 124.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/ShortPlatform.png");
	go->AddComponent<dae::RectCollider2DComponent>(16.0f, 4.0f);
	go->SetTag(make_sdbm_hash("Platform"));
	go->SetWorldPosition(160.0f, 140.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/ShortPlatform.png");
	go->AddComponent<dae::RectCollider2DComponent>(16.0f, 4.0f);
	go->SetTag(make_sdbm_hash("Platform"));
	go->SetWorldPosition(160.0f, 160.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/ShortPlatform.png");
	go->AddComponent<dae::RectCollider2DComponent>(16.0f, 4.0f);
	go->SetTag(make_sdbm_hash("Platform"));
	go->SetWorldPosition(160.0f, 196.0f);
	scene.Add(std::move(go));

	// Column x = 163
	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/Ladder.png");
	go->AddComponent<dae::RectCollider2DComponent>(10.0f, 16.0f);
	go->SetTag(make_sdbm_hash("Ladder"));
	go->SetWorldPosition(163.0f, 40.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/Ladder.png");
	go->AddComponent<dae::RectCollider2DComponent>(10.0f, 16.0f);
	go->SetTag(make_sdbm_hash("Ladder"));
	go->SetWorldPosition(163.0f, 56.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/Ladder.png");
	go->AddComponent<dae::RectCollider2DComponent>(10.0f, 16.0f);
	go->SetTag(make_sdbm_hash("Ladder"));
	go->SetWorldPosition(163.0f, 76.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/Ladder.png");
	go->AddComponent<dae::RectCollider2DComponent>(10.0f, 16.0f);
	go->SetTag(make_sdbm_hash("Ladder"));
	go->SetWorldPosition(163.0f, 92.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/ShortLadder.png");
	go->AddComponent<dae::RectCollider2DComponent>(10.0f, 12.0f);
	go->SetTag(make_sdbm_hash("Ladder"));
	go->SetWorldPosition(163.0f, 112.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/ShortLadder.png");
	go->AddComponent<dae::RectCollider2DComponent>(10.0f, 12.0f);
	go->SetTag(make_sdbm_hash("Ladder"));
	go->SetWorldPosition(163.0f, 128.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/Ladder.png");
	go->AddComponent<dae::RectCollider2DComponent>(10.0f, 16.0f);
	go->SetTag(make_sdbm_hash("Ladder"));
	go->SetWorldPosition(163.0f, 144.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/Ladder.png");
	go->AddComponent<dae::RectCollider2DComponent>(10.0f, 16.0f);
	go->SetTag(make_sdbm_hash("Ladder"));
	go->SetWorldPosition(163.0f, 164.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/Ladder.png");
	go->AddComponent<dae::RectCollider2DComponent>(10.0f, 16.0f);
	go->SetTag(make_sdbm_hash("Ladder"));
	go->SetWorldPosition(163.0f, 180.0f);
	scene.Add(std::move(go));

	// Column x = 176
	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/WidePlatform.png");
	go->AddComponent<dae::RectCollider2DComponent>(32.0f, 4.0f);
	go->SetTag(make_sdbm_hash("Platform"));
	go->SetWorldPosition(176.0f, 36.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/WidePlatform.png");
	go->AddComponent<dae::RectCollider2DComponent>(32.0f, 4.0f);
	go->SetTag(make_sdbm_hash("Platform"));
	go->SetWorldPosition(176.0f, 72.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/WidePlatform.png");
	go->AddComponent<dae::RectCollider2DComponent>(32.0f, 4.0f);
	go->SetTag(make_sdbm_hash("Platform"));
	go->SetWorldPosition(176.0f, 108.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/WidePlatform.png");
	go->AddComponent<dae::RectCollider2DComponent>(32.0f, 4.0f);
	go->SetTag(make_sdbm_hash("Platform"));
	go->SetWorldPosition(176.0f, 140.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/WidePlatform.png");
	go->AddComponent<dae::RectCollider2DComponent>(32.0f, 4.0f);
	go->SetTag(make_sdbm_hash("Platform"));
	go->SetWorldPosition(176.0f, 196.0f);
	scene.Add(std::move(go));

	// Column x = 187

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/ShortLadder.png");
	go->AddComponent<dae::RectCollider2DComponent>(10.0f, 12.0f);
	go->SetTag(make_sdbm_hash("Ladder"));
	go->SetWorldPosition(187.0f, 112.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/Ladder.png");
	go->AddComponent<dae::RectCollider2DComponent>(10.0f, 16.0f);
	go->SetTag(make_sdbm_hash("Ladder"));
	go->SetWorldPosition(187.0f, 124.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/Ladder.png");
	go->AddComponent<dae::RectCollider2DComponent>(10.0f, 16.0f);
	go->SetTag(make_sdbm_hash("Ladder"));
	go->SetWorldPosition(187.0f, 144.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/Ladder.png");
	go->AddComponent<dae::RectCollider2DComponent>(10.0f, 16.0f);
	go->SetTag(make_sdbm_hash("Ladder"));
	go->SetWorldPosition(187.0f, 160.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/Ladder.png");
	go->AddComponent<dae::RectCollider2DComponent>(10.0f, 16.0f);
	go->SetTag(make_sdbm_hash("Ladder"));
	go->SetWorldPosition(187.0f, 176.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/ShortLadder.png");
	go->AddComponent<dae::RectCollider2DComponent>(10.0f, 12.0f);
	go->SetTag(make_sdbm_hash("Ladder"));
	go->SetWorldPosition(187.0f, 184.0f);
	scene.Add(std::move(go));

	// Column x = 208
	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/ShortPlatform.png");
	go->AddComponent<dae::RectCollider2DComponent>(16.0f, 4.0f);
	go->SetTag(make_sdbm_hash("Platform"));
	go->SetWorldPosition(208.0f, 36.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/ShortPlatform.png");
	go->AddComponent<dae::RectCollider2DComponent>(16.0f, 4.0f);
	go->SetTag(make_sdbm_hash("Platform"));
	go->SetWorldPosition(208.0f, 72.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/ShortPlatform.png");
	go->AddComponent<dae::RectCollider2DComponent>(16.0f, 4.0f);
	go->SetTag(make_sdbm_hash("Platform"));
	go->SetWorldPosition(208.0f, 108.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/ShortPlatform.png");
	go->AddComponent<dae::RectCollider2DComponent>(16.0f, 4.0f);
	go->SetTag(make_sdbm_hash("Platform"));
	go->SetWorldPosition(208.0f, 140.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/ShortPlatform.png");
	go->AddComponent<dae::RectCollider2DComponent>(16.0f, 4.0f);
	go->SetTag(make_sdbm_hash("Platform"));
	go->SetWorldPosition(208.0f, 196.0f);
	scene.Add(std::move(go));

	// Column x = 211
	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/Ladder.png");
	go->AddComponent<dae::RectCollider2DComponent>(10.0f, 16.0f);
	go->SetTag(make_sdbm_hash("Ladder"));
	go->SetWorldPosition(211.0f, 40.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/Ladder.png");
	go->AddComponent<dae::RectCollider2DComponent>(10.0f, 16.0f);
	go->SetTag(make_sdbm_hash("Ladder"));
	go->SetWorldPosition(211.0f, 56.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/Ladder.png");
	go->AddComponent<dae::RectCollider2DComponent>(10.0f, 16.0f);
	go->SetTag(make_sdbm_hash("Ladder"));
	go->SetWorldPosition(211.0f, 76.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/Ladder.png");
	go->AddComponent<dae::RectCollider2DComponent>(10.0f, 16.0f);
	go->SetTag(make_sdbm_hash("Ladder"));
	go->SetWorldPosition(211.0f, 92.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/Ladder.png");
	go->AddComponent<dae::RectCollider2DComponent>(10.0f, 16.0f);
	go->SetTag(make_sdbm_hash("Ladder"));
	go->SetWorldPosition(211.0f, 144.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/Ladder.png");
	go->AddComponent<dae::RectCollider2DComponent>(10.0f, 16.0f);
	go->SetTag(make_sdbm_hash("Ladder"));
	go->SetWorldPosition(211.0f, 160.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/Ladder.png");
	go->AddComponent<dae::RectCollider2DComponent>(10.0f, 16.0f);
	go->SetTag(make_sdbm_hash("Ladder"));
	go->SetWorldPosition(211.0f, 176.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/ShortLadder.png");
	go->AddComponent<dae::RectCollider2DComponent>(10.0f, 12.0f);
	go->SetTag(make_sdbm_hash("Ladder"));
	go->SetWorldPosition(211.0f, 184.0f);
	scene.Add(std::move(go));






	// First burger x = 32

	go = std::make_unique<dae::GameObject>();
	go->SetTag(make_sdbm_hash("BurgerPart"));
	go->AddComponent<dae::BurgerPartComponent>("BurgerParts/BurgerBunTop.png", 8);
	go->AddComponent<dae::RectCollider2DComponent>(32.0f, 7.0f)->SetShouldTriggerEvents(true);
	go->SetWorldPosition(32.0f, 70.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->SetTag(make_sdbm_hash("BurgerPart"));
	go->AddComponent<dae::BurgerPartComponent>("BurgerParts/Lettuce.png", 8);
	go->AddComponent<dae::RectCollider2DComponent>(32.0f, 7.0f)->SetShouldTriggerEvents(true);
	go->SetWorldPosition(32.0f, 106.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->SetTag(make_sdbm_hash("BurgerPart"));
	go->AddComponent<dae::BurgerPartComponent>("BurgerParts/Burger.png", 8);
	go->AddComponent<dae::RectCollider2DComponent>(32.0f, 7.0f)->SetShouldTriggerEvents(true);
	go->SetWorldPosition(32.0f, 158.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->SetTag(make_sdbm_hash("BurgerPart"));
	go->AddComponent<dae::BurgerPartComponent>("BurgerParts/BurgerBunBottom.png", 8);
	go->AddComponent<dae::RectCollider2DComponent>(32.0f, 7.0f)->SetShouldTriggerEvents(true);
	go->SetWorldPosition(32.0f, 194.0f);
	scene.Add(std::move(go));

	
	// Second burger x = 80

	go = std::make_unique<dae::GameObject>();
	go->SetTag(make_sdbm_hash("BurgerPart"));
	go->AddComponent<dae::BurgerPartComponent>("BurgerParts/BurgerBunTop.png", 8);
	go->AddComponent<dae::RectCollider2DComponent>(32.0f, 7.0f)->SetShouldTriggerEvents(true);
	go->SetWorldPosition(80, 34.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->SetTag(make_sdbm_hash("BurgerPart"));
	go->AddComponent<dae::BurgerPartComponent>("BurgerParts/Lettuce.png", 8);
	go->AddComponent<dae::RectCollider2DComponent>(32.0f, 7.0f)->SetShouldTriggerEvents(true);
	go->SetWorldPosition(80, 122.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->SetTag(make_sdbm_hash("BurgerPart"));
	go->AddComponent<dae::BurgerPartComponent>("BurgerParts/Burger.png", 8);
	go->AddComponent<dae::RectCollider2DComponent>(32.0f, 7.0f)->SetShouldTriggerEvents(true);
	go->SetWorldPosition(80, 158.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->SetTag(make_sdbm_hash("BurgerPart"));
	go->AddComponent<dae::BurgerPartComponent>("BurgerParts/BurgerBunBottom.png", 8);
	go->AddComponent<dae::RectCollider2DComponent>(32.0f, 7.0f)->SetShouldTriggerEvents(true);
	go->SetWorldPosition(80, 194.0f);
	scene.Add(std::move(go));

	// Third burger x = 128

	go = std::make_unique<dae::GameObject>();
	go->SetTag(make_sdbm_hash("BurgerPart"));
	go->AddComponent<dae::BurgerPartComponent>("BurgerParts/BurgerBunTop.png", 8);
	go->AddComponent<dae::RectCollider2DComponent>(32.0f, 7.0f)->SetShouldTriggerEvents(true);
	go->SetWorldPosition(128, 34.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->SetTag(make_sdbm_hash("BurgerPart"));
	go->AddComponent<dae::BurgerPartComponent>("BurgerParts/Lettuce.png", 8);
	go->AddComponent<dae::RectCollider2DComponent>(32.0f, 7.0f)->SetShouldTriggerEvents(true);
	go->SetWorldPosition(128, 70.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->SetTag(make_sdbm_hash("BurgerPart"));
	go->AddComponent<dae::BurgerPartComponent>("BurgerParts/Burger.png", 8);
	go->AddComponent<dae::RectCollider2DComponent>(32.0f, 7.0f)->SetShouldTriggerEvents(true);
	go->SetWorldPosition(128, 122.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->SetTag(make_sdbm_hash("BurgerPart"));
	go->AddComponent<dae::BurgerPartComponent>("BurgerParts/BurgerBunBottom.png", 8);
	go->AddComponent<dae::RectCollider2DComponent>(32.0f, 7.0f)->SetShouldTriggerEvents(true);
	go->SetWorldPosition(128, 194.0f);
	scene.Add(std::move(go));

	// Fourth burger x = 176

	go = std::make_unique<dae::GameObject>();
	go->SetTag(make_sdbm_hash("BurgerPart"));
	go->AddComponent<dae::BurgerPartComponent>("BurgerParts/BurgerBunTop.png", 8);
	go->AddComponent<dae::RectCollider2DComponent>(32.0f, 7.0f)->SetShouldTriggerEvents(true);
	go->SetWorldPosition(176, 34.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->SetTag(make_sdbm_hash("BurgerPart"));
	go->AddComponent<dae::BurgerPartComponent>("BurgerParts/Lettuce.png", 8);
	go->AddComponent<dae::RectCollider2DComponent>(32.0f, 7.0f)->SetShouldTriggerEvents(true);
	go->SetWorldPosition(176, 70.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->SetTag(make_sdbm_hash("BurgerPart"));
	go->AddComponent<dae::BurgerPartComponent>("BurgerParts/Burger.png", 8);
	go->AddComponent<dae::RectCollider2DComponent>(32.0f, 7.0f)->SetShouldTriggerEvents(true);
	go->SetWorldPosition(176, 106.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->SetTag(make_sdbm_hash("BurgerPart"));
	go->AddComponent<dae::BurgerPartComponent>("BurgerParts/BurgerBunBottom.png", 8);
	go->AddComponent<dae::RectCollider2DComponent>(32.0f, 7.0f)->SetShouldTriggerEvents(true);
	go->SetWorldPosition(176, 138.0f);
	scene.Add(std::move(go));

	// Plates

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/Plate.png");
	go->AddComponent<dae::RectCollider2DComponent>(38.0f, 5.0f);
	go->SetTag(make_sdbm_hash("Plate"));
	go->SetWorldPosition(29.0f, 235.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/Plate.png");
	go->AddComponent<dae::RectCollider2DComponent>(38.0f, 5.0f);
	go->SetTag(make_sdbm_hash("Plate"));
	go->SetWorldPosition(77.0f, 235.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/Plate.png");
	go->AddComponent<dae::RectCollider2DComponent>(38.0f, 5.0f);
	go->SetTag(make_sdbm_hash("Plate"));
	go->SetWorldPosition(125.0f, 235.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("Level/Plate.png");
	go->AddComponent<dae::RectCollider2DComponent>(38.0f, 5.0f);
	go->SetTag(make_sdbm_hash("Plate"));
	go->SetWorldPosition(173.0f, 235.0f);
	scene.Add(std::move(go));

	// x is 240 (the width of the game)
	// y is 240 (the height of the game)

	go = std::make_unique<dae::GameObject>();
	go->SetTag(make_sdbm_hash("EnemyRespawnPoint"));
	go->SetWorldPosition(0.0f, 20.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->SetTag(make_sdbm_hash("EnemyRespawnPoint"));
	go->SetWorldPosition(224.0f, 20.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->SetTag(make_sdbm_hash("EnemyRespawnPoint"));
	go->SetWorldPosition(0.0f, 180.0f);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	go->SetTag(make_sdbm_hash("EnemyRespawnPoint"));
	go->SetWorldPosition(224.0f, 180.0f);
	scene.Add(std::move(go));


	go = std::make_unique<dae::GameObject>();
	go->SetTag(make_sdbm_hash("PlayerRespawnPoint"));
	go->SetWorldPosition(112, 181);
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