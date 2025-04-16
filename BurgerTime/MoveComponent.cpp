#include "MoveComponent.h"

#include <iostream>

#include "GameObject.h"
#include "SpritesheetComponent.h"

dae::MoveComponent::MoveComponent(GameObject* ownerPtr, float maxSpeed) : ComponentBase(ownerPtr), m_MaxSpeed{ maxSpeed }
{
	m_SpritesheetComponentPtr = ownerPtr->GetComponent<SpritesheetComponent>();
}

void dae::MoveComponent::Update(float elapsedSec)
{
	if (m_Direction != glm::vec2{ 0.0f, 0.0f })
	{
		m_LastNonZeroDirection = m_Direction;
	}

	// Set the correct animation -- TEMPORARY IMPLEMENTATION
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

void dae::MoveComponent::Notify(const Event& event, GameObject*)
{
	if (event.id == make_sdbm_hash("OnCollisionStay"))
	{
		std::cout << "Colliding! \n";
	}

	if (event.id == make_sdbm_hash("OnCollisionEnter"))
	{
		std::cout << "Entered collision! \n";
	}

	if (event.id == make_sdbm_hash("OnCollisionExit"))
	{
		std::cout << "Exited collision! \n";
	}
}

void dae::MoveComponent::SetDirection(const glm::vec2& direction)
{
	if (m_Direction == glm::vec2{ 0.0f, 0.0f })
	{
		m_Direction = direction;
	}
}






