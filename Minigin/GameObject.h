#pragma once
#include <memory>
#include <vector>

#include "ComponentBase.h"
#include "Transform.h"

namespace dae
{
	class GameObject final : public std::enable_shared_from_this<GameObject>
	{
	public:
		void Update(float elapsedSec);
		void FixedUpdate(float fixedTimeStep);
		void Render() const;

		void SetPosition(float x, float y);

		void AddComponent(std::shared_ptr<ComponentBase> component);

		template <typename T>
		bool HasComponent() const
		{
			for (const auto& component : m_Components)
			{
				if (std::dynamic_pointer_cast<T>(component))
				{
					return true;
				}
			}
			return false;
		}

		template <typename T>
		std::weak_ptr<T> GetComponent() const
		{
			for (const auto& component : m_Components)
			{
				if (auto casted = std::dynamic_pointer_cast<T>(component))
				{
					return casted;
				}
			}
			return nullptr;
		}

		GameObject() = default;
		~GameObject();
		GameObject(const GameObject& other) = delete;
		GameObject(GameObject&& other) = delete;
		GameObject& operator=(const GameObject& other) = delete;
		GameObject& operator=(GameObject&& other) = delete;

	private:
		Transform m_Transform{};

		std::vector<std::shared_ptr<ComponentBase>> m_Components{};
	};
}
