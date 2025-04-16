#include "RectCollider2DComponent.h"
#include "GameObject.h"

std::vector<dae::RectCollider2DComponent*> dae::RectCollider2DComponent::m_Colliders{};

dae::RectCollider2DComponent::RectCollider2DComponent(GameObject* ownerPtr, float collisionRectWidth, float collisionRectHeight): ComponentBase(ownerPtr)
{
	auto& ownerPos = ownerPtr->GetWorldTransform().GetPosition();

	m_CollisionRect.posX = ownerPos.x;
	m_CollisionRect.posY = ownerPos.y;
	m_CollisionRect.width = collisionRectWidth;
	m_CollisionRect.height = collisionRectHeight;

	m_CollisionEnterEvent = std::make_unique<Subject>();
	m_CollisionExitEvent = std::make_unique<Subject>();
	m_CollisionStayEvent = std::make_unique<Subject>();

	m_Colliders.push_back(this);
}

void dae::RectCollider2DComponent::Update(float)
{
	auto& ownerPos = GetOwner()->GetWorldTransform().GetPosition();

	m_CollisionRect.posX = ownerPos.x + m_LocalTransform.GetPosition().x;
	m_CollisionRect.posY = ownerPos.y + m_LocalTransform.GetPosition().y;

	std::unordered_set<RectCollider2DComponent*> currentFrameCollisions;

	for (auto otherCollider : m_Colliders)
	{
		if (otherCollider == this)
		{
			continue;
		}

		if (!IsOverlapping(m_CollisionRect, otherCollider->m_CollisionRect))
		{
			continue;
		}

		currentFrameCollisions.insert(otherCollider);

		// Handles physical position updates
		// If turned on both, adjusts parent game objects position to prevent it from stepping over the other object
		if (m_ShouldCollide && otherCollider->m_ShouldCollide)
		{
			if (!m_IsStatic)
			{
				auto shiftVector = GetCollisionOverlapShift(m_CollisionRect, otherCollider->m_CollisionRect);

				GetOwner()->SetLocalPosition(ownerPos.x + shiftVector.x, ownerPos.y + shiftVector.y);
			}
		}

		if (!m_ShouldTriggerEvents)
		{
			continue;
		}

		if (!m_PreviousFrameCollisions.contains(otherCollider))
		{
			m_CollisionEnterEvent->NotifyObservers(Event(make_sdbm_hash("OnCollisionEnter")), otherCollider->GetOwner());
		}
		else
		{
			m_CollisionStayEvent->NotifyObservers(Event(make_sdbm_hash("OnCollisionStay")), otherCollider->GetOwner());
		}
	}

	if (m_ShouldTriggerEvents)
	{
		for (auto prevCollider : m_PreviousFrameCollisions)
		{
			if (!currentFrameCollisions.contains(prevCollider))
			{
				m_CollisionExitEvent->NotifyObservers(Event(make_sdbm_hash("OnCollisionExit")), prevCollider->GetOwner());
			}
		}
	}

	m_PreviousFrameCollisions = std::move(currentFrameCollisions);
}

void dae::RectCollider2DComponent::LateUpdate(float)
{
}

void dae::RectCollider2DComponent::Render() const
{
}

void dae::RectCollider2DComponent::SetLocalPosition(float x, float y)
{
	m_LocalTransform.SetPosition(x, y, 0.0f);

	auto& ownerPos = GetOwner()->GetWorldTransform().GetPosition();
	m_CollisionRect.posX = ownerPos.x + x;
	m_CollisionRect.posY = ownerPos.y + y;
}

void dae::RectCollider2DComponent::SetShouldCollide(bool shouldCollide)
{
	m_ShouldCollide = shouldCollide;
}

void dae::RectCollider2DComponent::SetShouldTriggerEvents(bool triggerEvents)
{
	m_ShouldTriggerEvents = triggerEvents;
}

void dae::RectCollider2DComponent::SetIsStatic(bool isStatic)
{
	m_IsStatic = isStatic;
}

bool dae::RectCollider2DComponent::IsOverlapping(const Rect& rect1, const Rect& rect2) const
{
	if (rect1.posX < rect2.posX + rect2.width &&
		rect1.posX + rect1.width > rect2.posX &&
		rect1.posY < rect2.posY + rect2.height &&
		rect1.posY + rect1.height > rect2.posY)
	{
		return true;
	}
	return false;
}

glm::vec2 dae::RectCollider2DComponent::GetCollisionOverlapShift(const Rect& rect1, const Rect& rect2) const
{
	constexpr float epsilon = 0.001f;

	// Finding the earliest boundary on the right and the latest boundary on the left to see for overlap, by getting the width of the overlap 
	float overlapX = std::min(rect1.posX + rect1.width, rect2.posX + rect2.width) - std::max(rect1.posX, rect2.posX);

	// Same, but for vertical 
	float overlapY = std::min(rect1.posY + rect1.height, rect2.posY + rect2.height) - std::max(rect1.posY, rect2.posY);

	// Check if valid
	if (overlapX <= epsilon || overlapY <= epsilon)
	{
		return { 0, 0 };
	}

	overlapX -= epsilon;
	overlapY -= epsilon;

	if (overlapX < overlapY)
	{
		// If it is to the left, move left
		if (rect1.posX < rect2.posX)
		{
			return { -overlapX, 0 };
		}
		else
		{
			return { overlapX, 0 };
		}
	}
	else
	{
		if (rect1.posY < rect2.posY)
		{
			return { 0, -overlapY };
		}
		else
		{
			return { 0, overlapY };
		}
	}
}


dae::Subject* dae::RectCollider2DComponent::GetCollisionEnterEvent() const
{
	return m_CollisionEnterEvent.get();
}

dae::Subject* dae::RectCollider2DComponent::GetCollisionExitEvent() const
{
	return m_CollisionExitEvent.get();
}

dae::Subject* dae::RectCollider2DComponent::GetCollisionStayEvent() const
{
	return m_CollisionStayEvent.get();
}













