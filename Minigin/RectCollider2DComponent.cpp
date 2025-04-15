#include "RectCollider2DComponent.h"

#include "GameObject.h"

dae::RectCollider2DComponent::RectCollider2DComponent(GameObject* ownerPtr, int collisionRectWidth, int collisionRectHeight): ComponentBase(ownerPtr)
{
	m_CollisionRect.width = collisionRectWidth;
	m_CollisionRect.height = collisionRectHeight;

	m_CollisionEnterEvent = std::make_unique<Subject>();
	m_CollisionExitEvent = std::make_unique<Subject>();
	m_CollisionStayEvent = std::make_unique<Subject>();
}

void dae::RectCollider2DComponent::Update(float)
{
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

bool dae::RectCollider2DComponent::GetShouldTriggerEvents() const
{
	return m_ShouldTriggerEvents;
}

bool dae::RectCollider2DComponent::GetShouldCollide() const
{
	return m_ShouldCollide;
}

void dae::RectCollider2DComponent::SetShouldCollide(bool shouldCollide)
{
	m_ShouldCollide = shouldCollide;
}

void dae::RectCollider2DComponent::SetShouldTriggerEvents(bool triggerEvents)
{
	m_ShouldTriggerEvents = triggerEvents;
}









