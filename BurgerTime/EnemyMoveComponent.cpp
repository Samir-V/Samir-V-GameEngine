#include "EnemyMoveComponent.h"

#include <iostream>

#include "GameObject.h"

dae::EnemyMoveComponent::EnemyMoveComponent(GameObject* ownerPtr, const std::vector<GameObject*>& players, float maxSpeed) :
	ComponentBase(ownerPtr)
	, m_Players{players}
	, m_CurrentHorizontalDirective{}
	, m_CurrentVerticalDirective{}
	, m_Velocity{}
	, m_Direction{}
	, m_MaxSpeed{maxSpeed}
{
}

dae::EnemyMoveComponent::~EnemyMoveComponent()
{
}


void dae::EnemyMoveComponent::Start()
{
	const auto [horDirective, verDirective] = CalculateDirectionDirectives();

	if (horDirective == HorizontalDirective::Right)
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
	assert(!m_Players.empty());

	auto worldPos = GetOwner()->GetWorldTransform().GetPosition();

	auto activePlayers = m_Players | std::views::filter([](const GameObject* player) {
		return player->IsActive();
		});

	if (activePlayers.empty())
	{
		return { m_CurrentHorizontalDirective, m_CurrentVerticalDirective };
	}

	const auto closestPlayer = *std::ranges::min_element(activePlayers,
	                                                     [&worldPos](GameObject* a, GameObject* b) {
		                                                     const auto diffA = a->GetWorldTransform().GetPosition() - worldPos;
		                                                     const auto diffB = b->GetWorldTransform().GetPosition() - worldPos;
		                                                     return (diffA.x * diffA.x + diffA.y * diffA.y) < (diffB.x * diffB.x + diffB.y * diffB.y);
	                                                     });

	const auto diff = closestPlayer->GetWorldTransform().GetPosition() - worldPos;

	if (diff.x >= 0.0f)
	{
		m_CurrentHorizontalDirective = HorizontalDirective::Right;
	}
	else
	{
		m_CurrentHorizontalDirective = HorizontalDirective::Left;
	}

	if (diff.y > 0.0f)
	{
		m_CurrentVerticalDirective = VerticalDirective::Down;
	}	
	else if (diff.y < 0.0f)
	{
		m_CurrentVerticalDirective = VerticalDirective::Up;
	}
	else
	{
		m_CurrentVerticalDirective = VerticalDirective::None;
	}

	return { m_CurrentHorizontalDirective, m_CurrentVerticalDirective };
}
