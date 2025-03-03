#pragma once
#include "SceneManager.h"
#include "GameObject.h"

namespace dae
{
	class Scene final
	{
		friend Scene& SceneManager::CreateScene(const std::string& name);
	public:
		void Add(std::unique_ptr<GameObject> object);
		void Remove(std::unique_ptr<GameObject> object);
		void RemoveAll();

		void Update(float elapsedSec);
		void LateUpdate(float elapsedSec);
		void Render() const;
		void RenderImGui();

		~Scene();
		Scene(const Scene& other) = delete;
		Scene(Scene&& other) = delete;
		Scene& operator=(const Scene& other) = delete;
		Scene& operator=(Scene&& other) = delete;

		const std::vector < std::unique_ptr<GameObject>>& GetGameObjects();

	private: 
		explicit Scene(const std::string& name);

		std::string m_Name;
		std::vector < std::unique_ptr<GameObject>> m_Objects{};

		static unsigned int m_idCounter; 
	};

}
