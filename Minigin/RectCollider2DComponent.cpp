#include "RectCollider2DComponent.h"
#include "GameObject.h"

std::vector<dae::RectCollider2DComponent*> dae::RectCollider2DComponent::m_Colliders{};

dae::RectCollider2DComponent::RectCollider2DComponent(GameObject* ownerPtr, int collisionRectWidth, int collisionRectHeight): ComponentBase(ownerPtr)
{
	m_CollisionRect.width = collisionRectWidth;
	m_CollisionRect.height = collisionRectHeight;

	m_CollisionEnterEvent = std::make_unique<Subject>();
	m_CollisionExitEvent = std::make_unique<Subject>();
	m_CollisionStayEvent = std::make_unique<Subject>();

	m_Colliders.push_back(this);
}

void dae::RectCollider2DComponent::Update(float)
{
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

		if (m_ShouldCollide)
		{
			// Handles physical position updates
		}

		if (m_ShouldTriggerEvents)
		{
			m_CollisionStayEvent->NotifyObservers(Event(make_sdbm_hash("OnCollisionStay")), otherCollider->GetOwner());
		}
	}
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
	m_CollisionRect.posX = static_cast<int>(ownerPos.x + x);
	m_CollisionRect.posY = static_cast<int>(ownerPos.y + y);
}

void dae::RectCollider2DComponent::SetShouldCollide(bool shouldCollide)
{
	m_ShouldCollide = shouldCollide;
}

void dae::RectCollider2DComponent::SetShouldTriggerEvents(bool triggerEvents)
{
	m_ShouldTriggerEvents = triggerEvents;
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










