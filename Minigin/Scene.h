#pragma once
#include "SceneManager.h"
#include "GameObject.h"

namespace svengine
{
	class Scene final
	{
		friend Scene& SceneManager::CreateDontDestroyOnLoadScene(const std::string& name);
		friend std::shared_ptr<Scene> SceneManager::CreateSceneForFactory(const std::string& name);
	public:
		void Add(std::unique_ptr<GameObject> object);
		void Remove(std::unique_ptr<GameObject> object);
		void RemoveAll();

		void Start();
		void Update(float elapsedSec);
		void LateUpdate(float elapsedSec);
		void Render() const;

		~Scene();
		Scene(const Scene& other) = delete;
		Scene(Scene&& other) = delete;
		Scene& operator=(const Scene& other) = delete;
		Scene& operator=(Scene&& other) = delete;

		const std::vector <std::unique_ptr<GameObject>>& GetGameObjects();
		std::vector<GameObject*> GetGameObjectsWithTag(Tag tag) const;
		const std::string& GetName() const;

		void ForcePendingDestroys();

	private: 
		explicit Scene(const std::string& name);

		std::string m_Name;
		std::vector < std::unique_ptr<GameObject>> m_Objects{};

		static unsigned int m_idCounter; 
	};

}
