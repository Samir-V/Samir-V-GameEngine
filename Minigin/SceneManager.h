#pragma once
#include <functional>
#include <string>
#include <memory>
#include <unordered_map>

#include "Singleton.h"

namespace svengine
{
	class Scene;
	class SceneManager final : public Singleton<SceneManager>
	{
	public:
		Scene& CreateDontDestroyOnLoadScene(const std::string& name);
		std::shared_ptr<Scene> CreateSceneForFactory(const std::string& name);
		Scene* GetSceneByName(const std::string& name);
		Scene* GetActiveScene() const;
		Scene* GetDontDestroyOnLoadScene() const;

		void RegisterSceneFactory(const std::string& name, std::function<std::shared_ptr<Scene>()> factory);

		void Start();
		void Update(float elapsedSec);
		void LateUpdate(float elapsedSec);
		void Render();
		void SetActiveScene(const std::string& name);

	private:
		friend class Singleton<SceneManager>;
		SceneManager() = default;
		std::unordered_map<std::string, std::shared_ptr<Scene>> m_Scenes;
		std::unordered_map<std::string, std::function<std::shared_ptr<Scene>()>> m_SceneFactories;
		Scene* m_ActiveScene{nullptr};
		Scene* m_DontDestroyOnLoadScene{nullptr};
	};
}
