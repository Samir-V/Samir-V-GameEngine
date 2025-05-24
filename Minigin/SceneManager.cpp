#include "SceneManager.h"
#include "Scene.h"

void dae::SceneManager::Start()
{
	for (auto& scene : m_Scenes)
	{
		scene.second->Start();
	}
}


void dae::SceneManager::Update(float elapsedSec)
{
	for(auto& scene : m_Scenes)
	{
		scene.second->Update(elapsedSec);
	}
}

void dae::SceneManager::LateUpdate(float elapsedSec)
{
	for (auto& scene : m_Scenes)
	{
		scene.second->LateUpdate(elapsedSec);
	}
}

void dae::SceneManager::Render()
{
	for (const auto& scene : m_Scenes)
	{
		scene.second->Render();
	}
}

dae::Scene& dae::SceneManager::CreateScene(const std::string& name)
{
	const auto& scene = std::shared_ptr<Scene>(new Scene(name));
	m_Scenes.insert({ name, scene });
	return *scene;
}

dae::Scene* dae::SceneManager::GetSceneByName(const std::string& name)
{
	auto it = std::ranges::find_if(m_Scenes, [&name](std::pair<std::string, std::shared_ptr<Scene>> pair)
		{
			return pair.first == name;
		});

	if (it != m_Scenes.end())
	{
		return it->second.get();
	}

	return nullptr;
}

