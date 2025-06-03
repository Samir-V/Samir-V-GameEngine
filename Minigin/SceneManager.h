#pragma once
#include <string>
#include <memory>
#include <unordered_map>

#include "Singleton.h"

namespace dae
{
	class Scene;
	class SceneManager final : public Singleton<SceneManager>
	{
	public:
		Scene& CreateScene(const std::string& name, bool dontDestroyOnLoad);
		Scene* GetSceneByName(const std::string& name);
		Scene* GetActiveScene() const;

		void Start();
		void Update(float elapsedSec);
		void LateUpdate(float elapsedSec);
		void Render();
		void SetActiveScene(const std::string& name);

	private:
		friend class Singleton<SceneManager>;
		SceneManager() = default;
		std::unordered_map<std::string, std::shared_ptr<Scene>> m_Scenes;
		Scene* m_ActiveScene{nullptr};
		Scene* m_DontDestroyOnLoadScene{nullptr};
	};
}
