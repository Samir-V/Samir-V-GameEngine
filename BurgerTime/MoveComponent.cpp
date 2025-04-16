#include "MoveComponent.h"

#include <iostream>

#include "GameObject.h"
#include "SpritesheetComponent.h"
#include "LevelPartComponent.h"
#include "RectCollider2DComponent.h"

dae::MoveComponent::MoveComponent(GameObject* ownerPtr, float maxSpeed) : ComponentBase(ownerPtr), m_MaxSpeed{ maxSpeed }
{
	m_SpritesheetComponentPtr = ownerPtr->GetComponent<SpritesheetComponent>();
	m_OwnerColliderPtr = ownerPtr->GetComponent<RectCollider2DComponent>();
}

void dae::MoveComponent::Update(float elapsedSec)
{
	// Walking opportunities checking

	auto ownerColliderRect = m_OwnerColliderPtr->GetCollisionRect();
	glm::vec2 rayOrigin{ ownerColliderRect.posX + ownerColliderRect.width / 2.0f, ownerColliderRect.posY + ownerColliderRect.height };
	glm::vec2 rayDirection = glm::vec2(0.0f, 1.0f);
	float rayLength = 2.0f;
	float hitDistance = 0.0f;

	m_CanGoHorizontally = false;
	glm::vec2 verticalCollisionShift{};

	for (auto currentPlatformCollider : m_CurrentPlatformsColliders)
	{
		if (currentPlatformCollider->RayIntersect(rayOrigin, rayDirection, rayLength, hitDistance))
		{
			//std::cout << "Raycast sees collision \n";
			verticalCollisionShift = RectCollider2DComponent::GetCollisionOverlapShift(ownerColliderRect, currentPlatformCollider->GetCollisionRect());
			m_CanGoHorizontally = true;
			break;
		}
	}

	m_CanGoVertically = false;

	/*rayOrigin = { ownerColliderRect.posX + ownerColliderRect.width / 2.0f, ownerColliderRect.posY + ownerColliderRect.height / 2.0f };
	rayLength = 10.0f;*/

	if (!m_CurrentPlatformsColliders.empty())
	{
		m_CanGoVertically = true;
	}

	// Movement prevention

	if (m_Direction.y != 0.0f && !m_CanGoVertically)
	{
		m_Direction = glm::vec2{ 0.0f, 0.0f };
		return;
	}

	if (m_Direction.x != 0.0f && !m_CanGoHorizontally)
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
		m_SpritesheetComponentPtr->Play("PPWalkingLeft.png");
	}
	else if (m_Direction.x > 0)
	{
		m_SpritesheetComponentPtr->Play("PPWalkingRight.png");
	}
	else if (m_Direction.y > 0)
	{
		m_SpritesheetComponentPtr->Play("PPWalkingDown.png");
	}
	else if (m_Direction.y < 0)
	{
		m_SpritesheetComponentPtr->Play("PPWalkingUp.png");
	}
	else
	{
		if (m_LastNonZeroDirection.x < 0)
		{
			m_SpritesheetComponentPtr->Play("PPIdleLeft.png");
		}
		else if (m_LastNonZeroDirection.x > 0)
		{
			m_SpritesheetComponentPtr->Play("PPIdleRight.png");
		}
		else if (m_LastNonZeroDirection.y > 0)
		{
			m_SpritesheetComponentPtr->Play("PPIdleDown.png");
		}
		else if (m_LastNonZeroDirection.y < 0)
		{
			m_SpritesheetComponentPtr->Play("PPIdleUp.png");
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
		std::cout << "Entered Collision! \n";

		auto levelPartComponent = observedGameObject->GetComponent<LevelPartComponent>();

		if (levelPartComponent)
		{
			switch (levelPartComponent->GetLevelPartType())
			{
			case LevelPartComponent::LevelPartType::Platform:
				m_CurrentPlatformsColliders.insert(observedGameObject->GetComponent<RectCollider2DComponent>());
				break;
			case LevelPartComponent::LevelPartType::Ladder:
				m_CurrentLadderColliders.insert(observedGameObject->GetComponent<RectCollider2DComponent>());
				break;
			}
			
		}
	}

	if (event.id == make_sdbm_hash("OnCollisionStay"))
	{
		/*std::cout << "Colliding! \n";

		bool isStandingOnPlatform{};
		constexpr float threshold{ 0.8f };

		for (auto currentPlatformCollider : m_CurrentPlatformsColliders)
		{
			auto platformColliderRect = currentPlatformCollider->GetCollisionRect();

			auto ownerCollider = GetOwner()->GetComponent<RectCollider2DComponent>();
			auto ownerColliderRect = ownerCollider->GetCollisionRect();

			float ownerBottomPos = ownerColliderRect.posY + ownerColliderRect.height;

			if (std::abs(ownerBottomPos - platformColliderRect.posY) < threshold)
			{
				isStandingOnPlatform = true;
				break;
			}
		}

		m_CanGoHorizontally = isStandingOnPlatform;*/
	}

	if (event.id == make_sdbm_hash("OnCollisionExit"))
	{
		std::cout << "Exited Collision! \n";

		auto levelPartComponent = observedGameObject->GetComponent<LevelPartComponent>();

		if (levelPartComponent)
		{
			switch (levelPartComponent->GetLevelPartType())
			{
			case LevelPartComponent::LevelPartType::Platform:
				m_CurrentPlatformsColliders.erase(observedGameObject->GetComponent<RectCollider2DComponent>());
				break;
			case LevelPartComponent::LevelPartType::Ladder:
				m_CurrentLadderColliders.erase(observedGameObject->GetComponent<RectCollider2DComponent>());
				break;
			}
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






