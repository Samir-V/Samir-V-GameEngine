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

		void AddComponent(std::unique_ptr<ComponentBase> component);

		GameObject() = default;
		~GameObject();
		GameObject(const GameObject& other) = delete;
		GameObject(GameObject&& other) = delete;
		GameObject& operator=(const GameObject& other) = delete;
		GameObject& operator=(GameObject&& other) = delete;

	private:
		Transform m_Transform{};

		std::vector<std::unique_ptr<ComponentBase>> m_Components{};
	};
}
