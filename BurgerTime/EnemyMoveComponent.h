#pragma once

#include "ComponentBase.h"
#include "Transform.h"

namespace dae
{
	class RectCollider2DComponent;

	class EnemyMoveComponent final : public ComponentBase
	{
	public:

		enum class HorizontalDirective
		{
			Left,
			Right
		};

		enum class VerticalDirective
		{
			Up,
			Down,
			None
		};

		struct MovingDirective
		{
			HorizontalDirective horDirective;
			VerticalDirective verDirective;
		};

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

		const glm::vec2& GetDirection() const;
		void SetDirection(const glm::vec2& direction);

		MovingDirective CalculateDirectionDirectives();

	private:

		GameObject* m_PlayerTargetPtr{};

		HorizontalDirective m_CurrentHorizontalDirective{};
		VerticalDirective m_CurrentVerticalDirective{};

		Transform m_LocalTransform{};
		glm::vec2 m_Velocity{};
		glm::vec2 m_Direction{};

		const float m_MaxSpeed{};
	};
}
