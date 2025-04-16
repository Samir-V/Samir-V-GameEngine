#pragma once
#include <set>

#include "ComponentBase.h"
#include "IObserver.h"
#include "Transform.h"

namespace dae
{
	class RectCollider2DComponent;
}

namespace dae
{
	class LevelPartComponent;
	class SpritesheetComponent;

	class MoveComponent final : public ComponentBase, public IObserver
	{
	public:

		MoveComponent(GameObject* ownerPtr, float maxSpeed);
		~MoveComponent() override = default;

		MoveComponent(const MoveComponent& other) = delete;
		MoveComponent(MoveComponent&& other) = delete;
		MoveComponent& operator=(const MoveComponent& other) = delete;
		MoveComponent& operator=(MoveComponent&& other) = delete;

		void Update(float elapsedSec) override;
		void LateUpdate(float elapsedSec) override;
		void Render() const override;

		void SetLocalPosition(float x, float y) override;
		void Notify(const Event& event, GameObject* observedGameObject) override;

		void SetDirection(const glm::vec2& direction);

	private:

		Transform m_LocalTransform{};
		glm::vec2 m_Velocity{};
		glm::vec2 m_Direction{};
		glm::vec2 m_LastNonZeroDirection{};

		bool m_CanGoHorizontally	{ false };
		bool m_CanGoVertically		{ true };

		std::set<RectCollider2DComponent*> m_CurrentPlatformsColliders;

		const float m_MaxSpeed{};

		SpritesheetComponent* m_SpritesheetComponentPtr{};
		RectCollider2DComponent* m_OwnerColliderPtr{};
	};
}

