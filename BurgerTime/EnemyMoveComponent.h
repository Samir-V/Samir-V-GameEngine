#pragma once

#include <set>

#include "ComponentBase.h"
#include "IObserver.h"
#include "Transform.h"

namespace dae
{
	class RectCollider2DComponent;

	class EnemyMoveComponent final : public ComponentBase, public IObserver
	{
	public:
		EnemyMoveComponent(GameObject* ownerPtr, GameObject* playerPtr, float maxSpeed);
		~EnemyMoveComponent() override = default;

		EnemyMoveComponent(const EnemyMoveComponent& other) = delete;
		EnemyMoveComponent(EnemyMoveComponent&& other) = delete;
		EnemyMoveComponent& operator=(const EnemyMoveComponent& other) = delete;
		EnemyMoveComponent& operator=(EnemyMoveComponent&& other) = delete;

		void Start() override;
		void Update(float elapsedSec) override;
		void LateUpdate(float elapsedSec) override;
		void Render() const override;

		void SetLocalPosition(float x, float y) override;
		void Notify(const Event& event, GameObject* observedGameObject) override;

	private:

		void CalculateDirectionDirectives();

		enum class HorizontalDirective
		{
			Left,
			Right
		};

		enum class VerticalDirective
		{
			Up,
			Down
		};

		GameObject* m_PlayerTargetPtr{};

		HorizontalDirective m_CurrentHorizontalDirective{};
		VerticalDirective m_CurrentVerticalDirective{};

		Transform m_LocalTransform{};
		glm::vec2 m_Velocity{};
		glm::vec2 m_Direction{};
		glm::vec2 m_LastNonZeroDirection{};

		const float m_MaxSpeed{};

		std::set<RectCollider2DComponent*> m_CurrentPlatformsColliders;
		std::set<RectCollider2DComponent*> m_CurrentLadderColliders;

		RectCollider2DComponent* m_OwnerColliderPtr{};
	};
}
