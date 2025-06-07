#include "MoveComponent.h"


#include "EnemyComponent.h"
#include "GameObject.h"
#include "PeterPepperComponent.h"
#include "RectCollider2DComponent.h"

dae::MoveComponent::MoveComponent(GameObject* ownerPtr, float maxSpeed) : ComponentBase(ownerPtr), m_MaxSpeed{ maxSpeed }
{
	m_OwnerColliderPtr = ownerPtr->GetComponent<RectCollider2DComponent>();
}

void dae::MoveComponent::Start()
{
	m_OwnerColliderPtr->GetCollisionEnterEvent()->AddObserver(this);
	m_OwnerColliderPtr->GetCollisionExitEvent()->AddObserver(this);
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

	//std::cout << pepperPos.x << " " << pepperPos.y << "\n";

	const auto newPepperPosX = pepperPos.x + m_Velocity.x * elapsedSec;
	const auto newPepperPosY = pepperPos.y + m_Velocity.y * elapsedSec;

	GetOwner()->SetWorldPosition(newPepperPosX, newPepperPosY);
}

void dae::MoveComponent::LateUpdate(float)
{
}

void dae::MoveComponent::Render() const
{
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

		if (observedGameObject->GetTag() == make_sdbm_hash("Enemy"))
		{
			auto enemyStateType = observedGameObject->GetComponent<EnemyComponent>()->GetCurrentStateType();

			if (enemyStateType != typeid(StunnedState) && enemyStateType != typeid(FallingState))
			{
				auto peterPepperComp = GetOwner()->GetComponent<PeterPepperComponent>();
				peterPepperComp->ChangeState(std::make_unique<DyingState>());
			}
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

const glm::vec2& dae::MoveComponent::GetLastDirection() const
{
	return m_LastNonZeroDirection;
}

const glm::vec2& dae::MoveComponent::GetVelocity() const
{
	return m_Velocity;
}

// This handles input for movement
void dae::MoveComponent::SetDirection(const glm::vec2& direction)
{
	if (m_Direction == glm::vec2{ 0.0f, 0.0f })
	{
		m_Direction = direction;
	}
}

void dae::MoveComponent::Reset()
{
	// to avoid referencing dangling pointers when reloading the level
	m_CurrentPlatformsColliders.clear();
	m_CurrentLadderColliders.clear();
}


void dae::MoveComponent::UpdateVerticalMovement()
{
	if (m_Direction.y == 0.0f)
	{
		return;
	}

	if (m_Direction.y < 0.0f && !m_CurrentLadderColliders.empty())
	{
		auto randomLadderCollider = *m_CurrentLadderColliders.begin();
		auto randomLadderColliderRect = randomLadderCollider->GetCollisionRect();

		auto ladderWidth = randomLadderColliderRect.width;
		auto widthDifferenceSplit = (m_OwnerColliderPtr->GetCollisionRect().width - ladderWidth) / 2.0f;

		float xWorld = randomLadderColliderRect.posX - widthDifferenceSplit;

		auto characterPos = GetOwner()->GetWorldTransform().GetPosition();

		GetOwner()->SetWorldPosition(xWorld, characterPos.y);

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

	bool ladderFound{ false };
	GameObject* ladderGameObject{ nullptr };

	for (auto intersectedGameObject : intersectedGameObjects)
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
		auto ladderWidth = ladderGameObject->GetComponent<RectCollider2DComponent>()->GetCollisionRect().width;
		auto widthDifferenceSplit = (ownerColliderRect.width - ladderWidth) / 2.0f;

		float xWorld = ladderGameObject->GetWorldTransform().GetPosition().x - widthDifferenceSplit;

		auto& characterPos = GetOwner()->GetWorldTransform().GetPosition();

		GetOwner()->SetWorldPosition(xWorld, characterPos.y);
	}
}

glm::vec2 dae::MoveComponent::UpdateHorizontalMovement()
{
	m_CanGoHorizontally = false;

	if (m_Direction.x != 0.0f)
	{
		auto ownerColliderRect = m_OwnerColliderPtr->GetCollisionRect();
		constexpr glm::vec2 rayDirection = glm::vec2(0.0f, 1.0f);

		const glm::vec2 leftOrigin = { ownerColliderRect.posX, ownerColliderRect.posY + ownerColliderRect.height };
		const glm::vec2 rightOrigin = { ownerColliderRect.posX + ownerColliderRect.width, ownerColliderRect.posY + ownerColliderRect.height };

		for (auto currentPlatformCollider : m_CurrentPlatformsColliders)
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







