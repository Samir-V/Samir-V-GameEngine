#pragma once
#include <memory>
#include <vector>
#include <ranges>
#include <algorithm>

#include "ComponentBase.h"
#include "Transform.h"

namespace dae
{
	class GameObject final
	{
	public:

		GameObject() = default;
		~GameObject();
		GameObject(const GameObject& other) = delete;
		GameObject(GameObject&& other) = delete;
		GameObject& operator=(const GameObject& other) = delete;
		GameObject& operator=(GameObject&& other) = delete;

		void Update(float elapsedSec);
		void LateUpdate(float elapsedSec);
		void Render() const;

		void SetLocalPosition(float x, float y);
		void SetLocalPosition(const glm::vec3& pos);
		void SetParent(GameObject* newParentPtr, bool worldPositionStays);
		std::vector<GameObject*>& GetChildrenVector();

		Transform GetWorldTransform();
		void SetPositionIsDirty();

		template <typename T, typename... Args>
		T* AddComponent(Args&&... args)
		{
			auto component = std::make_unique<T>(this, std::forward<Args>(args)...);
			auto rawPtr = component.get();
			m_Components.push_back(std::move(component));

			return rawPtr;
		}

		template <typename T>
		bool HasComponent() const
		{
			return std::ranges::any_of(m_Components, [](const auto& component)
			{
				return dynamic_cast<T*>(component.get()) != nullptr;
			});
		}

		template <typename T>
		T* GetComponent() const
		{
			auto it = std::ranges::find_if(m_Components, [](const auto& component) 
				{
					return dynamic_cast<T*>(component.get()) != nullptr;
				});

			if (it != m_Components.end())
			{
				return static_cast<T*>((*it).get());
			}

			return nullptr;
		}

		template <typename T>
		void DeleteComponent() const
		{
			auto it = std::ranges::find_if(m_Components, [](const auto& component)
				{
					return dynamic_cast<T*>(component.get()) != nullptr;
				});

			if (it != m_Components.end())
			{
				auto component = static_cast<T*>((*it).get());

				component->Destroy();
			}
		}

	private:

		bool IsNotInChildren(GameObject* gameObject) const;
		void UpdateWorldPosition();

		bool m_PositionIsDirty{};

		Transform m_LocalTransform{};
		Transform m_WorldTransform{};
		GameObject* m_Parent{};

		std::vector<std::unique_ptr<ComponentBase>> m_Components{};
		std::vector<GameObject*> m_Children{};
	};
}
