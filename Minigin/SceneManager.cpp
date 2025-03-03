#include "SceneManager.h"
#include "Scene.h"

void dae::SceneManager::Update(float elapsedSec)
{
	for(auto& scene : m_Scenes)
	{
		scene->Update(elapsedSec);
	}
}

void dae::SceneManager::LateUpdate(float elapsedSec)
{
	for (auto& scene : m_Scenes)
	{
		scene->LateUpdate(elapsedSec);
	}
}

void dae::SceneManager::Render()
{
	for (const auto& scene : m_Scenes)
	{
		scene->Render();
	}
}

void dae::SceneManager::RenderImGui()
{
	for (const auto& scene : m_Scenes)
	{
		scene->RenderImGui();
	}
}


dae::Scene& dae::SceneManager::CreateScene(const std::string& name)
{
	const auto& scene = std::shared_ptr<Scene>(new Scene(name));
	m_Scenes.push_back(scene);
	return *scene;
}
