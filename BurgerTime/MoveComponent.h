#pragma once
#include <set>

#include "ComponentBase.h"
#include "IObserver.h"

namespace dae
{
	class RectCollider2DComponent;

	class MoveComponent final : public ComponentBase, public IObserver
	{
	public:

		MoveComponent(GameObject* ownerPtr, float maxSpeed);
		~MoveComponent() override = default;

		MoveComponent(const MoveComponent& other) = delete;
		MoveComponent(MoveComponent&& other) = delete;
		MoveComponent& operator=(const MoveComponent& other) = delete;
		MoveComponent& operator=(MoveComponent&& other) = delete;

		void Start() override;
		void Update(float elapsedSec) override;
		void LateUpdate(float elapsedSec) override;
		void Render() const override;

		void Notify(const Event& event, GameObject* observedGameObject) override;

		void SetDirection(const glm::vec2& direction);

		void Reset();

		const glm::vec2& GetVelocity() const;
		const glm::vec2& GetLastDirection() const;

	private:

		void UpdateVerticalMovement();
		glm::vec2 UpdateHorizontalMovement();

		glm::vec2 m_Velocity;
		glm::vec2 m_Direction;
		glm::vec2 m_LastNonZeroDirection;

		bool m_CanGoHorizontally;
		bool m_CanGoVertically;

		std::set<RectCollider2DComponent*> m_CurrentPlatformsColliders;
		std::set<RectCollider2DComponent*> m_CurrentLadderColliders;
		std::set<RectCollider2DComponent*> m_CurrentEnemyColliders;

		const float m_MaxSpeed;

		RectCollider2DComponent* m_OwnerColliderPtr;
	};
}

