#include "MoveComponent.h"

#include <iostream>

#include "GameObject.h"
#include "SpritesheetComponent.h"
#include "RectCollider2DComponent.h"

dae::MoveComponent::MoveComponent(GameObject* ownerPtr, float maxSpeed) : ComponentBase(ownerPtr), m_MaxSpeed{ maxSpeed }
{
	m_SpritesheetComponentPtr = ownerPtr->GetComponent<SpritesheetComponent>();
	m_OwnerColliderPtr = ownerPtr->GetComponent<RectCollider2DComponent>();
}
	
void dae::MoveComponent::Update(float elapsedSec)
{
	// Walking opportunities checking

	if (m_CurrentPlatformsColliders.empty())
	{
		m_CanGoHorizontally = false;
	}

	if (m_CurrentLadderColliders.empty())
	{
		m_CanGoVertically = false;
	}

	glm::vec2 verticalCollisionShift = UpdateHorizontalMovement();
	UpdateVerticalMovement();

	// Movement prevention

	if ((m_Direction.y != 0.0f && !m_CanGoVertically) || (m_Direction.x != 0.0f && !m_CanGoHorizontally))
	{
		m_Direction = glm::vec2{ 0.0f, 0.0f };
		return;
	}

	// If movement allowed
	if (m_Direction.x != 0.0f && m_CanGoHorizontally)
	{
		// Apply the shift for the character to stay on the platform
		auto& ownerPos = GetOwner()->GetWorldTransform().GetPosition();
		GetOwner()->SetLocalPosition(ownerPos.x + verticalCollisionShift.x, ownerPos.y + verticalCollisionShift.y);
	}


	// Set the correct animation -- TEMPORARY IMPLEMENTATION
	if (m_Direction != glm::vec2{ 0.0f, 0.0f })
	{
		m_LastNonZeroDirection = m_Direction;
	}

	if (m_Direction.x < 0)
	{
		m_SpritesheetComponentPtr->Play(make_sdbm_hash("PPWalkingLeft"));
	}
	else if (m_Direction.x > 0)
	{
		m_SpritesheetComponentPtr->Play(make_sdbm_hash("PPWalkingRight"));
	}
	else if (m_Direction.y > 0)
	{
		m_SpritesheetComponentPtr->Play(make_sdbm_hash("PPWalkingDown"));
	}
	else if (m_Direction.y < 0)
	{
		m_SpritesheetComponentPtr->Play(make_sdbm_hash("PPWalkingUp"));
	}
	else
	{
		if (m_LastNonZeroDirection.x < 0)
		{
			m_SpritesheetComponentPtr->Play(make_sdbm_hash("PPIdleLeft"));
		}
		else if (m_LastNonZeroDirection.x > 0)
		{
			m_SpritesheetComponentPtr->Play(make_sdbm_hash("PPIdleRight"));
		}
		else if (m_LastNonZeroDirection.y > 0)
		{
			m_SpritesheetComponentPtr->Play(make_sdbm_hash("PPIdleDown"));
		}
		else if (m_LastNonZeroDirection.y < 0)
		{
			m_SpritesheetComponentPtr->Play(make_sdbm_hash("PPIdleUp"));
		}
	}


	// Position update

	m_Velocity = m_Direction * m_MaxSpeed;

	m_Direction = glm::vec2{ 0.0f, 0.0f };

	const auto pepperPos = GetOwner()->GetWorldTransform().GetPosition();

	const auto newPepperPosX = pepperPos.x + m_Velocity.x * elapsedSec;
	const auto newPepperPosY = pepperPos.y + m_Velocity.y * elapsedSec;

	GetOwner()->SetLocalPosition(newPepperPosX, newPepperPosY);
}

void dae::MoveComponent::LateUpdate(float)
{
	
}

void dae::MoveComponent::Render() const
{
	
}

void dae::MoveComponent::SetLocalPosition(float x, float y)
{
	m_LocalTransform.SetPosition(x, y, 0.0f);
}

void dae::MoveComponent::Notify(const Event& event, GameObject* observedGameObject)
{
	if (event.id == make_sdbm_hash("OnCollisionEnter"))
	{
		if (observedGameObject->GetTag() == make_sdbm_hash("Platform"))
		{
			m_CurrentPlatformsColliders.insert(observedGameObject->GetComponent<RectCollider2DComponent>());
		}

		if (observedGameObject->GetTag() == make_sdbm_hash("Ladder"))
		{
			m_CurrentLadderColliders.insert(observedGameObject->GetComponent<RectCollider2DComponent>());
		}
	}

	if (event.id == make_sdbm_hash("OnCollisionStay"))
	{
		
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

void dae::MoveComponent::SetDirection(const glm::vec2& direction)
{
	if (m_Direction == glm::vec2{ 0.0f, 0.0f })
	{
		m_Direction = direction;
	}
}

void dae::MoveComponent::UpdateVerticalMovement()
{
	if (m_Direction.y == 0.0f)
	{
		return;
	}

	if (m_Direction.y < 0.0f && !m_CurrentLadderColliders.empty())
	{
		m_CanGoVertically = true;
		return;
	}

	auto ownerColliderRect = m_OwnerColliderPtr->GetCollisionRect();
	glm::vec2 rayOrigin{ ownerColliderRect.posX + ownerColliderRect.width / 2.0f, ownerColliderRect.posY + ownerColliderRect.height };
	glm::vec2 rayDirection{ 0.0f, 1.0f };
	float rayLength;

	if (m_Direction.y < 0.0f)
	{
		rayLength = 3.0f;
	}
	else
	{
		rayLength = 5.0f;
	}

	auto intersectedGameObjects = RectCollider2DComponent::GetRayIntersectedGameObjects(rayOrigin, rayDirection, rayLength);

	bool ladderFound = std::ranges::any_of(intersectedGameObjects, [](const GameObject* gameObject) {
		return gameObject->GetTag() == make_sdbm_hash("Ladder");
		});

	m_CanGoVertically = ladderFound;
	m_CanGoHorizontally = false;
}

glm::vec2 dae::MoveComponent::UpdateHorizontalMovement()
{
	if (m_Direction.x != 0.0f && !m_CanGoHorizontally)
	{
		auto ownerColliderRect = m_OwnerColliderPtr->GetCollisionRect();
		glm::vec2 rayOrigin{ ownerColliderRect.posX + ownerColliderRect.width / 2.0f, ownerColliderRect.posY + ownerColliderRect.height };
		glm::vec2 rayDirection = glm::vec2(0.0f, 1.0f);
		float rayLength = 2.0f;

		for (auto currentPlatformCollider : m_CurrentPlatformsColliders)
		{
			if (currentPlatformCollider->RayIntersect(rayOrigin, rayDirection, rayLength))
			{
				m_CanGoHorizontally = true;
				return RectCollider2DComponent::GetCollisionOverlapShift(ownerColliderRect, currentPlatformCollider->GetCollisionRect());
			}
		}
	}

	return {};
}







