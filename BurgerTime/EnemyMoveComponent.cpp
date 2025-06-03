#include "EnemyMoveComponent.h"

#include <iostream>

#include "GameObject.h"

dae::EnemyMoveComponent::EnemyMoveComponent(GameObject* ownerPtr, GameObject* playerPtr, float maxSpeed) : ComponentBase(ownerPtr), m_PlayerTargetPtr{playerPtr}, m_MaxSpeed { maxSpeed }
{
}

void dae::EnemyMoveComponent::Start()
{
	auto directives = CalculateDirectionDirectives();

	if (directives.horDirective == HorizontalDirective::Right)
	{
		SetDirection({ 1.0f, 0.0f });
	}
	else
	{
		SetDirection({ -1.0f, 0.0f });
	}
}

void dae::EnemyMoveComponent::Update(float elapsedSec)
{
	// Position update

	m_Velocity = m_Direction * m_MaxSpeed;

	const auto& enemyPos = GetOwner()->GetWorldTransform().GetPosition();

	const auto newEnemyPosX = enemyPos.x + m_Velocity.x * elapsedSec;
	const auto newEnemyPosY = enemyPos.y + m_Velocity.y * elapsedSec;

	GetOwner()->SetWorldPosition(newEnemyPosX, newEnemyPosY);
}

void dae::EnemyMoveComponent::LateUpdate(float)
{
}

void dae::EnemyMoveComponent::Render() const
{
}

const glm::vec2& dae::EnemyMoveComponent::GetDirection() const
{
	return m_Direction;
}

void dae::EnemyMoveComponent::SetDirection(const glm::vec2& direction)
{
	m_Direction = direction;
}

dae::EnemyMoveComponent::MovingDirective dae::EnemyMoveComponent::CalculateDirectionDirectives()
{
	auto playerWorldPos = m_PlayerTargetPtr->GetWorldTransform().GetPosition();
	auto worldPos = GetOwner()->GetWorldTransform().GetPosition();

	auto diff = playerWorldPos - worldPos;

	if (diff.x >= 0.0f)
	{
		m_CurrentHorizontalDirective = HorizontalDirective::Right;
		std::cout << "Horizontal - Right" << "\n";
	}
	else
	{
		m_CurrentHorizontalDirective = HorizontalDirective::Left;
		std::cout << "Horizontal - Left" << "\n";
	}

	if (diff.y > 0.0f)
	{
		m_CurrentVerticalDirective = VerticalDirective::Down;
		std::cout << "Vertical - Down" << "\n";
	}	
	else if (diff.y < 0.0f)
	{
		m_CurrentVerticalDirective = VerticalDirective::Up;
		std::cout << "Vertical - Up" << "\n";
	}
	else
	{
		m_CurrentVerticalDirective = VerticalDirective::None;
		std::cout << "Vertical - None" << "\n";
	}

	return { m_CurrentHorizontalDirective, m_CurrentVerticalDirective };
}
