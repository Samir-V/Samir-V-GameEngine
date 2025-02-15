#include <SDL.h>

#if _DEBUG
// ReSharper disable once CppUnusedIncludeDirective
#if __has_include(<vld.h>)
#include <vld.h>
#endif
#endif

#include "FPSComponent.h"
#include "Minigin.h"
#include "SceneManager.h"
#include "ResourceManager.h"
#include "TextComponent.h"
#include "GameObject.h"
#include "Scene.h"
#include "Texture2DComponent.h"

void load()
{
	auto& scene = dae::SceneManager::GetInstance().CreateScene("MainScene");

	auto go = std::make_unique<dae::GameObject>();
	go->AddComponent<dae::Texture2DComponent>("background.tga");
	scene.Add(std::move(go));	

	go = std::make_unique<dae::GameObject>();
	auto textureComp = go->AddComponent<dae::Texture2DComponent>("logo.tga");
	textureComp->SetPosition(216, 180);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	auto font = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 28);
	auto textComp = go->AddComponent<dae::TextComponent>("Programming 4 Assignment", font);
	textComp->SetPosition(140, 40);
	scene.Add(std::move(go));

	go = std::make_unique<dae::GameObject>();
	textComp = go->AddComponent<dae::TextComponent>("60.0 FPS", font);
	textComp->SetPosition(10, 10);
	go->AddComponent<dae::FPSComponent>(textComp);

	//go->DeleteComponent<dae::TextComponent>();
	//go->DeleteComponent<dae::FPSComponent>();

	scene.Add(std::move(go));
}

int main(int, char*[]) {
	dae::Minigin engine("../Data/");
	engine.Run(load);
    return 0;
}