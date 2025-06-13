#pragma once

#include <vector>

#include "ComponentBase.h"

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

		EnemyMoveComponent(GameObject* ownerPtr, const std::vector<GameObject*>& players, float maxSpeed);
		~EnemyMoveComponent() override;

		EnemyMoveComponent(const EnemyMoveComponent& other) = delete;
		EnemyMoveComponent(EnemyMoveComponent&& other) = delete;
		EnemyMoveComponent& operator=(const EnemyMoveComponent& other) = delete;
		EnemyMoveComponent& operator=(EnemyMoveComponent&& other) = delete;

		void Start() override;
		void Update(float elapsedSec) override;
		void LateUpdate(float elapsedSec) override;
		void Render() const override;

		const glm::vec2& GetDirection() const;
		void SetDirection(const glm::vec2& direction);
		void Reset();

		MovingDirective CalculateDirectionDirectives();

	private:

		std::vector<GameObject*> m_Players;

		HorizontalDirective m_CurrentHorizontalDirective;
		VerticalDirective m_CurrentVerticalDirective;

		glm::vec2 m_Velocity;
		glm::vec2 m_Direction;

		const float m_MaxSpeed;
	};
}
