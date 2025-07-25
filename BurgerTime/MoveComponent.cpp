#include "MoveComponent.h"

#include "EnemyComponent.h"
#include "GameObject.h"
#include "PeterPepperComponent.h"
#include "RectCollider2DComponent.h"

svengine::MoveComponent::MoveComponent(GameObject* ownerPtr, float maxSpeed) :
	ComponentBase(ownerPtr)
	, m_Velocity{}
	, m_Direction{}
	, m_LastNonZeroDirection{}
	, m_CanGoHorizontally{false}
	, m_CanGoVertically{false}
	, m_MaxSpeed{maxSpeed}
{
	m_OwnerColliderPtr = ownerPtr->GetComponent<RectCollider2DComponent>();
}

void svengine::MoveComponent::Start()
{
	m_OwnerColliderPtr->GetCollisionEnterEvent()->AddObserver(this);
	m_OwnerColliderPtr->GetCollisionStayEvent()->AddObserver(this);
	m_OwnerColliderPtr->GetCollisionExitEvent()->AddObserver(this);
}

void svengine::MoveComponent::Update(float elapsedSec)
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

	const glm::vec2 verticalCollisionShift = UpdateHorizontalMovement();
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
		GetOwner()->SetWorldPosition(ownerPos.x + verticalCollisionShift.x, ownerPos.y + verticalCollisionShift.y);
	}

	if (m_Direction != glm::vec2{ 0.0f, 0.0f })
	{
		m_LastNonZeroDirection = m_Direction;
	}

	// Position update

	m_Velocity = m_Direction * m_MaxSpeed;

	m_Direction = glm::vec2{ 0.0f, 0.0f };

	const auto& pepperPos = GetOwner()->GetWorldTransform().GetPosition();

	const auto newPepperPosX = pepperPos.x + m_Velocity.x * elapsedSec;
	const auto newPepperPosY = pepperPos.y + m_Velocity.y * elapsedSec;

	GetOwner()->SetWorldPosition(newPepperPosX, newPepperPosY);
}

void svengine::MoveComponent::LateUpdate(float)
{
}

void svengine::MoveComponent::Render() const
{
}

void svengine::MoveComponent::Notify(const Event& event, GameObject* gameObjectCausingEvent)
{
	if (event.id == make_sdbm_hash("OnCollisionEnter"))
	{
		if (gameObjectCausingEvent->GetTag() == make_sdbm_hash("Platform"))
		{
			m_CurrentPlatformsColliders.insert(gameObjectCausingEvent->GetComponent<RectCollider2DComponent>());
		}

		if (gameObjectCausingEvent->GetTag() == make_sdbm_hash("Ladder"))
		{
			m_CurrentLadderColliders.insert(gameObjectCausingEvent->GetComponent<RectCollider2DComponent>());
		}

		if (gameObjectCausingEvent->GetTag() == make_sdbm_hash("Enemy"))
		{
			m_CurrentEnemyColliders.insert(gameObjectCausingEvent->GetComponent<RectCollider2DComponent>());
		}
	}

	if (event.id == make_sdbm_hash("OnCollisionStay"))
	{
		if (gameObjectCausingEvent->GetTag() == make_sdbm_hash("Enemy"))
		{
			for (const auto currentEnemyCollider : m_CurrentEnemyColliders)
			{
				const auto playerColliderRect = m_OwnerColliderPtr->GetCollisionRect();
				const auto enemyColliderRect = currentEnemyCollider->GetCollisionRect();

				const float playerCenterY = playerColliderRect.posY + playerColliderRect.height * 0.5f;
				const float enemyCenterY = enemyColliderRect.posY + enemyColliderRect.height * 0.5f;

				const float verticalThreshold = playerColliderRect.height * 0.25f;
				const float absDY = std::abs(playerCenterY - enemyCenterY);

				if (absDY > verticalThreshold)
				{
					continue;
				}

				auto enemyStateType = gameObjectCausingEvent->GetComponent<EnemyComponent>()->GetCurrentStateType();

				if (enemyStateType != typeid(EnemyStunnedState) && enemyStateType != typeid(EnemyFallingState))
				{
					const auto peterPepperComp = GetOwner()->GetComponent<PeterPepperComponent>();
					peterPepperComp->ChangeState(std::make_unique<DyingState>());
					break;
				}
			}
		}
	}

	if (event.id == make_sdbm_hash("OnCollisionExit"))
	{
		if (gameObjectCausingEvent->GetTag() == make_sdbm_hash("Platform"))
		{
			m_CurrentPlatformsColliders.erase(gameObjectCausingEvent->GetComponent<RectCollider2DComponent>());
		}

		if (gameObjectCausingEvent->GetTag() == make_sdbm_hash("Ladder"))
		{
			m_CurrentLadderColliders.erase(gameObjectCausingEvent->GetComponent<RectCollider2DComponent>());
		}

		if (gameObjectCausingEvent->GetTag() == make_sdbm_hash("Enemy"))
		{
			m_CurrentEnemyColliders.erase(gameObjectCausingEvent->GetComponent<RectCollider2DComponent>());
		}
	}
}

const glm::vec2& svengine::MoveComponent::GetLastDirection() const
{
	return m_LastNonZeroDirection;
}

const glm::vec2& svengine::MoveComponent::GetVelocity() const
{
	return m_Velocity;
}

// This handles input for movement
void svengine::MoveComponent::SetDirection(const glm::vec2& direction)
{
	if (m_Direction == glm::vec2{ 0.0f, 0.0f })
	{
		m_Direction = direction;
	}
}

void svengine::MoveComponent::Reset()
{
	// to avoid referencing dangling pointers when reloading the level
	m_CurrentPlatformsColliders.clear();
	m_CurrentLadderColliders.clear();
	m_CurrentEnemyColliders.clear();
	m_CanGoHorizontally = false;
	m_CanGoVertically = false;
}


void svengine::MoveComponent::UpdateVerticalMovement()
{
	if (m_Direction.y == 0.0f)
	{
		return;
	}

	if (m_Direction.y < 0.0f && !m_CurrentLadderColliders.empty())
	{
		const auto randomLadderCollider = *m_CurrentLadderColliders.begin();
		const auto randomLadderColliderRect = randomLadderCollider->GetCollisionRect();

		const auto ladderWidth = randomLadderColliderRect.width;
		const auto widthDifferenceSplit = (m_OwnerColliderPtr->GetCollisionRect().width - ladderWidth) / 2.0f;

		const float xWorld = randomLadderColliderRect.posX - widthDifferenceSplit;

		const auto characterPos = GetOwner()->GetWorldTransform().GetPosition();

		GetOwner()->SetWorldPosition(xWorld, characterPos.y);

		m_CanGoVertically = true;
		return;
	}

	const auto ownerColliderRect = m_OwnerColliderPtr->GetCollisionRect();
	const glm::vec2 rayOrigin{ ownerColliderRect.posX + ownerColliderRect.width / 2.0f, ownerColliderRect.posY + ownerColliderRect.height };
	const glm::vec2 rayDirection{ 0.0f, 1.0f };
	float rayLength;

	if (m_Direction.y < 0.0f)
	{
		rayLength = 3.0f;
	}
	else
	{
		rayLength = 5.0f;
	}

	const auto intersectedGameObjects = RectCollider2DComponent::GetRayIntersectedGameObjects(rayOrigin, rayDirection, rayLength);

	bool ladderFound{ false };
	GameObject* ladderGameObject{ nullptr };

	for (const auto intersectedGameObject : intersectedGameObjects)
	{
		if (intersectedGameObject->GetTag() == make_sdbm_hash("Ladder"))
		{
			ladderFound = true;
			ladderGameObject = intersectedGameObject;
		}
	}

	m_CanGoVertically = ladderFound;
	m_CanGoHorizontally = false;

	if (ladderGameObject != nullptr)
	{
		const auto ladderWidth = ladderGameObject->GetComponent<RectCollider2DComponent>()->GetCollisionRect().width;
		const auto widthDifferenceSplit = (ownerColliderRect.width - ladderWidth) / 2.0f;

		const float xWorld = ladderGameObject->GetWorldTransform().GetPosition().x - widthDifferenceSplit;

		auto& characterPos = GetOwner()->GetWorldTransform().GetPosition();

		GetOwner()->SetWorldPosition(xWorld, characterPos.y);
	}
}

glm::vec2 svengine::MoveComponent::UpdateHorizontalMovement()
{
	m_CanGoHorizontally = false;

	if (m_Direction.x != 0.0f)
	{
		const auto ownerColliderRect = m_OwnerColliderPtr->GetCollisionRect();
		constexpr glm::vec2 rayDirection = glm::vec2(0.0f, 1.0f);

		const glm::vec2 leftOrigin = { ownerColliderRect.posX, ownerColliderRect.posY + ownerColliderRect.height };
		const glm::vec2 rightOrigin = { ownerColliderRect.posX + ownerColliderRect.width, ownerColliderRect.posY + ownerColliderRect.height };

		for (const auto currentPlatformCollider : m_CurrentPlatformsColliders)
		{
			constexpr float rayLength = 2.0f;
			if (m_Direction.x < 0.0f && currentPlatformCollider->RayIntersect(leftOrigin, rayDirection, rayLength))
			{
				m_CanGoHorizontally = true;
				return RectCollider2DComponent::GetCollisionOverlapShift(ownerColliderRect, currentPlatformCollider->GetCollisionRect());
			}

			if (m_Direction.x > 0.0f && currentPlatformCollider->RayIntersect(rightOrigin, rayDirection, rayLength))
			{
				m_CanGoHorizontally = true;
				return RectCollider2DComponent::GetCollisionOverlapShift(ownerColliderRect, currentPlatformCollider->GetCollisionRect());
			}
		}
	}

	return {};
}







