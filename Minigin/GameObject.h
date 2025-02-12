#pragma once
#include <memory>
#include <vector>

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
		void FixedUpdate(float fixedTimeStep);
		void Render() const;

		void SetPosition(float x, float y);

		void AddComponent(std::unique_ptr<ComponentBase> component);

		template <typename T>
		bool HasComponent() const
		{
			for (const auto& component : m_Components)
			{
				if (dynamic_cast<T*>(component.get()))
				{
					return true;
				}
			}
			return false;
		}

		template <typename T>
		T* GetComponent() const
		{
			for (const auto& component : m_Components)
			{
				if (T* casted = dynamic_cast<T*>(component.get()))
				{
					return casted;
				}
			}
			return nullptr;
		}

	private:

		Transform m_Transform{};

		std::vector<std::unique_ptr<ComponentBase>> m_Components{};
	};
}
