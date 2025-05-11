#include "EnemyMoveComponent.h"

#include "EnemyStates.h"
#include "GameObject.h"
#include "RectCollider2DComponent.h"

dae::EnemyMoveComponent::EnemyMoveComponent(GameObject* ownerPtr, GameObject* playerPtr, float maxSpeed) : ComponentBase(ownerPtr), m_PlayerTargetPtr{playerPtr}, m_MaxSpeed { maxSpeed }
{
	m_OwnerColliderPtr = ownerPtr->GetComponent<RectCollider2DComponent>();
}

void dae::EnemyMoveComponent::Start()
{
}

void dae::EnemyMoveComponent::Update(float elapsedSec)
{
	//if (GetEnemyState == WalkingState)

	// Raycast down 5.0f

	//CalculateDirectionDirectives()

	/*if (m_CurrentVerticalDirective == VerticalDirective::Up)
	{
		m_Direction = { 0.0f, -1.0f };
	}
	else
	{
		m_Direction = { 0.0f, 1.0f };
	}*/

	//ChangeState(ClimbingState);


	// Position update

	m_Velocity = m_Direction * m_MaxSpeed;

	m_Direction = glm::vec2{ 0.0f, 0.0f };

	const auto& pepperPos = GetOwner()->GetWorldTransform().GetPosition();

	const auto newPepperPosX = pepperPos.x + m_Velocity.x * elapsedSec;
	const auto newPepperPosY = pepperPos.y + m_Velocity.y * elapsedSec;

	GetOwner()->SetWorldPosition(newPepperPosX, newPepperPosY);
}

void dae::EnemyMoveComponent::LateUpdate(float)
{
}

void dae::EnemyMoveComponent::Render() const
{
}

void dae::EnemyMoveComponent::SetLocalPosition(float x, float y)
{
	m_LocalTransform.SetPosition(x, y, 0.0f);
}

void dae::EnemyMoveComponent::Notify(const Event& event, GameObject* observedGameObject)
{
	if (event.id == make_sdbm_hash("OnCollisionEnter"))
	{
		if (observedGameObject->GetTag() == make_sdbm_hash("Platform"))
		{
			m_CurrentPlatformsColliders.insert(observedGameObject->GetComponent<RectCollider2DComponent>());

			//if (GetEnemyState == ClimbingState)

			CalculateDirectionDirectives();

			if (m_CurrentHorizontalDirective == HorizontalDirective::Right)
			{
				m_Direction = { 1.0f, 0.0f };
			}
			else
			{
				m_Direction = { -1.0f, 0.0f };
			}

			//ChangeState(FinishedClimbingState)???
			// Maybe should be fully done in the OnCollisionStay, so that it would not start walking to the side right after registering
			// Raycast of 2.0f down?
		}

		if (observedGameObject->GetTag() == make_sdbm_hash("Ladder"))
		{
			m_CurrentLadderColliders.insert(observedGameObject->GetComponent<RectCollider2DComponent>());

			//if (GetEnemyState == WalkingState)

			CalculateDirectionDirectives();

			if (m_CurrentVerticalDirective == VerticalDirective::Up)
			{
				m_Direction = { 0.0f, -1.0f };
			}
			else
			{
				m_Direction = { 0.0f, 1.0f };
			}

			//ChangeState(ClimbingState);
		}
	}

	if (event.id == make_sdbm_hash("OnCollisionExit"))
	{
		if (observedGameObject->GetTag() == make_sdbm_hash("Platform"))
		{
			m_CurrentPlatformsColliders.erase(observedGameObject->GetComponent<RectCollider2DComponent>());
		}

		if (observedGameObject->GetTag() == make_sdbm_hash("Ladder"))
		{
			m_CurrentLadderColliders.erase(observedGameObject->GetComponent<RectCollider2DComponent>());
		}
	}
}

const glm::vec2& dae::EnemyMoveComponent::GetVelocity() const
{
	return m_Velocity;
}

dae::EnemyMoveComponent::EnemyType dae::EnemyMoveComponent::GetEnemyType() const
{
	return m_EnemyType;
}


void dae::EnemyMoveComponent::CalculateDirectionDirectives()
{
	auto playerWorldPos = m_PlayerTargetPtr->GetWorldTransform().GetPosition();
	auto worldPos = GetOwner()->GetWorldTransform().GetPosition();

	auto diff = playerWorldPos - worldPos;

	if (diff.x >= 0.0f)
	{
		m_CurrentHorizontalDirective = HorizontalDirective::Right;
	}
	else
	{
		m_CurrentHorizontalDirective = HorizontalDirective::Left;
	}

	if (diff.y >= 0.0f)
	{
		m_CurrentVerticalDirective = VerticalDirective::Down;
	}
	else
	{
		m_CurrentVerticalDirective = VerticalDirective::Up;
	}
}
