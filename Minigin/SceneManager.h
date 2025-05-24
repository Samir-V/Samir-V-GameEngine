#pragma once
#include <vector>
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
		Scene& CreateScene(const std::string& name);
		Scene* GetSceneByName(const std::string& name);

		void Start();
		void Update(float elapsedSec);
		void LateUpdate(float elapsedSec);
		void Render();
	private:
		friend class Singleton<SceneManager>;
		SceneManager() = default;
		std::unordered_map<std::string, std::shared_ptr<Scene>> m_Scenes;
		//std::<std::shared_ptr<Scene>> m_Scenes;
	};
}
