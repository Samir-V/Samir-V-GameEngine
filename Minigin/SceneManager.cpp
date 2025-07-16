#include "SceneManager.h"

#include "RectCollider2DComponent.h"
#include "Scene.h"

void svengine::SceneManager::Start()
{
	m_DontDestroyOnLoadScene->Start();

	if (!m_ActiveScene)
	{
		return;
	}

	m_ActiveScene->Start();
}


void svengine::SceneManager::Update(float elapsedSec)
{
	m_DontDestroyOnLoadScene->Update(elapsedSec);

	if (!m_ActiveScene)
	{
		return;
	}

	m_ActiveScene->Update(elapsedSec);
}

void svengine::SceneManager::LateUpdate(float elapsedSec)
{
	m_DontDestroyOnLoadScene->LateUpdate(elapsedSec);

	if (!m_ActiveScene)
	{
		return;
	}

	m_ActiveScene->LateUpdate(elapsedSec);
}

void svengine::SceneManager::Render()
{
	if (!m_ActiveScene)
	{
		return;
	}

	m_ActiveScene->Render();

	m_DontDestroyOnLoadScene->Render();
}

svengine::Scene& svengine::SceneManager::CreateDontDestroyOnLoadScene(const std::string& name)
{
	assert(!m_DontDestroyOnLoadScene);

	auto scenePtr = std::shared_ptr<Scene>(new Scene(name));
	m_Scenes.emplace(name, scenePtr);
	m_DontDestroyOnLoadScene = scenePtr.get();
	return *scenePtr;
}

std::shared_ptr<svengine::Scene> svengine::SceneManager::CreateSceneForFactory(const std::string& name)
{
	const auto& scene = std::shared_ptr<Scene>(new Scene(name));
	return scene;
}


svengine::Scene* svengine::SceneManager::GetSceneByName(const std::string& name)
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

svengine::Scene* svengine::SceneManager::GetActiveScene() const
{
	return m_ActiveScene;
}

svengine::Scene* svengine::SceneManager::GetDontDestroyOnLoadScene() const
{
	return m_DontDestroyOnLoadScene;
}


void svengine::SceneManager::SetActiveScene(const std::string& name)
{
	if (m_ActiveScene)
	{
		m_ActiveScene->ForcePendingDestroys();
		RectCollider2DComponent::RemoveAllCollidersFromScene(m_ActiveScene);
	}

	auto fit = m_SceneFactories.find(name);
	if (fit != m_SceneFactories.end())
	{
		m_Scenes[name] = fit->second();
		m_ActiveScene = m_Scenes[name].get();
		m_ActiveScene->Start();
	}
	else
	{
		auto it = m_Scenes.find(name);
		if (it != m_Scenes.end())
		{
			m_ActiveScene = it->second.get();
			m_ActiveScene->Start();
		}
	}

	if (m_DontDestroyOnLoadScene)
	{
		m_DontDestroyOnLoadScene->Start();
	}
}

void svengine::SceneManager::RegisterSceneFactory(const std::string& name, std::function<std::shared_ptr<Scene>()> factory)
{
	m_SceneFactories[name] = std::move(factory);
}

