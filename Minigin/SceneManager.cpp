#include "SceneManager.h"
#include "Scene.h"

void dae::SceneManager::Start()
{
	if (!m_ActiveScene)
	{
		return;
	}

	m_ActiveScene->Start();

	/*for (auto& scene : m_Scenes)
	{
		scene.second->Start();
	}*/
}


void dae::SceneManager::Update(float elapsedSec)
{
	if (!m_ActiveScene)
	{
		return;
	}

	m_ActiveScene->Update(elapsedSec);

	/*for(auto& scene : m_Scenes)
	{
		scene.second->Update(elapsedSec);
	}*/
}

void dae::SceneManager::LateUpdate(float elapsedSec)
{
	if (!m_ActiveScene)
	{
		return;
	}

	m_ActiveScene->LateUpdate(elapsedSec);

	/*for (auto& scene : m_Scenes)
	{
		scene.second->LateUpdate(elapsedSec);
	}*/
}

void dae::SceneManager::Render()
{
	if (!m_ActiveScene)
	{
		return;
	}

	m_ActiveScene->Render();

	/*for (const auto& scene : m_Scenes)
	{
		scene.second->Render();
	}*/
}

dae::Scene& dae::SceneManager::CreateScene(const std::string& name)
{
	const auto& scene = std::shared_ptr<Scene>(new Scene(name));
	m_Scenes.insert({ name, scene });
	return *scene;
}

dae::Scene* dae::SceneManager::GetSceneByName(const std::string& name)
{
	auto it = std::ranges::find_if(m_Scenes, [&name](const std::pair<std::string, std::shared_ptr<Scene>>& pair)
		{
			return pair.first == name;
		});

	if (it != m_Scenes.end())
	{
		return it->second.get();
	}

	return nullptr;
}

dae::Scene* dae::SceneManager::GetActiveScene() const
{
	return m_ActiveScene;
}

void dae::SceneManager::SetActiveScene(const std::string& name)
{
	auto it = std::ranges::find_if(m_Scenes, [&name](const std::pair<std::string, std::shared_ptr<Scene>>& pair)
		{
			return pair.first == name;
		});

	if (it != m_Scenes.end())
	{
		m_ActiveScene = it->second.get();
		m_ActiveScene->Start();
	}
}


