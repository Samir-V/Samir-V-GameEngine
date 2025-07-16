#include "RectCollider2DComponent.h"

#include "GameObject.h"
#include "SceneManager.h"

std::vector<svengine::RectCollider2DComponent*> svengine::RectCollider2DComponent::m_Colliders{};

svengine::RectCollider2DComponent::RectCollider2DComponent(GameObject* ownerPtr, float collisionRectWidth, float collisionRectHeight):
	ComponentBase(ownerPtr)
	, m_CollisionEnterEvent {std::make_unique<Subject>()}
	, m_CollisionExitEvent {std::make_unique<Subject>()}
	, m_CollisionStayEvent {std::make_unique<Subject>()}
{
	auto& ownerPos = ownerPtr->GetWorldTransform().GetPosition();

	m_CollisionRect.posX = ownerPos.x;
	m_CollisionRect.posY = ownerPos.y;
	m_CollisionRect.width = collisionRectWidth;
	m_CollisionRect.height = collisionRectHeight;

	m_Colliders.push_back(this);
}

svengine::RectCollider2DComponent::~RectCollider2DComponent()
{
	std::erase(m_Colliders, this);

	if (m_PreviousFrameCollisions.contains(this))
	{
		m_PreviousFrameCollisions.erase(this);
	}

	for (const auto other : m_Colliders)
	{
		if (other->m_PreviousFrameCollisions.contains(this))
		{
			other->m_PreviousFrameCollisions.erase(this);
		}
	}
}


void svengine::RectCollider2DComponent::Start()
{
}

void svengine::RectCollider2DComponent::Update(float)
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

		if (otherCollider->GetOwner() == nullptr)
		{
			continue;
		}

		if (otherCollider->GetOwner()->GetParentScene() != SceneManager::GetInstance().GetActiveScene() 
			&& otherCollider->GetOwner()->GetParentScene() != SceneManager::GetInstance().GetDontDestroyOnLoadScene())
		{
			continue;
		}

		if (!otherCollider->IsActive() || !otherCollider->GetOwner()->IsActive())
		{
			continue;
		}

		if (!IsOverlapping(m_CollisionRect, otherCollider->m_CollisionRect))
		{
			continue;
		}

		currentFrameCollisions.insert(otherCollider);

		if (m_ShouldCollide && otherCollider->m_ShouldCollide)
		{
			auto& otherPos = otherCollider->GetOwner()->GetWorldTransform().GetPosition();

			const auto shiftVector = GetCollisionOverlapShift(m_CollisionRect, otherCollider->m_CollisionRect);

			if (!m_IsStatic && !otherCollider->m_IsStatic)
			{
				GetOwner()->SetLocalPosition(ownerPos.x + shiftVector.x * 0.5f, ownerPos.y + shiftVector.y * 0.5f);

				otherCollider->GetOwner()->SetLocalPosition(otherPos.x - shiftVector.x * 0.5f, otherPos.y - shiftVector.y * 0.5f);
			}
			else if(!m_IsStatic)
			{
				GetOwner()->SetLocalPosition(ownerPos.x + shiftVector.x, ownerPos.y + shiftVector.y);
			}
		}

		if (!m_ShouldTriggerEvents)
		{
			continue;
		}

		if (!m_PreviousFrameCollisions.contains(otherCollider) && otherCollider->GetOwner() != nullptr)
		{
			m_CollisionEnterEvent->NotifyObservers(Event(make_sdbm_hash("OnCollisionEnter")), otherCollider->GetOwner());
		}
		else if (otherCollider->GetOwner() != nullptr)
		{
			m_CollisionStayEvent->NotifyObservers(Event(make_sdbm_hash("OnCollisionStay")), otherCollider->GetOwner());
		}
	}

	if (m_ShouldTriggerEvents)
	{
		for (auto prevCollider : m_PreviousFrameCollisions)
		{
			if (!currentFrameCollisions.contains(prevCollider) && prevCollider->GetOwner() != nullptr 
				&& prevCollider->GetOwner()->IsActive()
				&& !prevCollider->GetOwner()->IsMarkedToDestroy())
			{
				m_CollisionExitEvent->NotifyObservers(Event(make_sdbm_hash("OnCollisionExit")), prevCollider->GetOwner());
			}
		}
	}

	m_PreviousFrameCollisions = std::move(currentFrameCollisions);
}

void svengine::RectCollider2DComponent::LateUpdate(float)
{
}

void svengine::RectCollider2DComponent::Render() const
{
}

void svengine::RectCollider2DComponent::SetLocalPosition(float x, float y)
{
	m_LocalTransform.SetPosition(x, y, 0.0f);

	auto& ownerPos = GetOwner()->GetWorldTransform().GetPosition();
	m_CollisionRect.posX = ownerPos.x + x;
	m_CollisionRect.posY = ownerPos.y + y;
}

void svengine::RectCollider2DComponent::SetShouldCollide(bool shouldCollide)
{
	m_ShouldCollide = shouldCollide;
}

void svengine::RectCollider2DComponent::SetShouldTriggerEvents(bool triggerEvents)
{
	m_ShouldTriggerEvents = triggerEvents;
}

void svengine::RectCollider2DComponent::SetIsStatic(bool isStatic)
{
	m_IsStatic = isStatic;
}

bool svengine::RectCollider2DComponent::IsOverlapping(const Rect& rect1, const Rect& rect2)
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

glm::vec2 svengine::RectCollider2DComponent::GetCollisionOverlapShift(const Rect& rect1, const Rect& rect2)
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

bool svengine::RectCollider2DComponent::RayIntersect(const glm::vec2& rayOrigin, const glm::vec2& rayDirection, float rayLength) const
{
	const float minX = m_CollisionRect.posX;
	const float maxX = m_CollisionRect.posX + m_CollisionRect.width;
	const float minY = m_CollisionRect.posY;
	const float maxY = m_CollisionRect.posY + m_CollisionRect.height;

	float distance{};

	glm::vec2 invDir{};
	if (rayDirection.x != 0.0f)
	{
		invDir.x = 1.0f / rayDirection.x;
	}
	else
	{
		invDir.x = std::numeric_limits<float>::max();
	}

	if (rayDirection.y != 0.0f)
	{
		invDir.y = 1.0f / rayDirection.y;
	}
	else
	{
		invDir.y = std::numeric_limits<float>::max();
	}

	const float t1 = (minX - rayOrigin.x) * invDir.x;
	const float t2 = (maxX - rayOrigin.x) * invDir.x;
	const float t3 = (minY - rayOrigin.y) * invDir.y;
	const float t4 = (maxY - rayOrigin.y) * invDir.y;

	const float tmin = std::max(std::min(t1, t2), std::min(t3, t4));
	const float tmax = std::min(std::max(t1, t2), std::max(t3, t4));

	if (tmax < 0 || tmin > tmax)
	{
		return false;
	}

	if (tmin < 0.0f)
	{
		distance = 0.0f;
	}
	else
	{
		distance = tmin;
	}

	if (distance > rayLength)
	{
		return false;
	}

	return true;
}

std::unordered_set<svengine::GameObject*> svengine::RectCollider2DComponent::GetRayIntersectedGameObjects(const glm::vec2& rayOrigin, const glm::vec2& rayDirection, float rayLength)
{
	std::unordered_set<GameObject*> intersectedGameObjects{};

	for (const auto collider : m_Colliders)
	{
		if (collider->RayIntersect(rayOrigin, rayDirection, rayLength))
		{
			intersectedGameObjects.insert(collider->GetOwner());
		}
	}

	return intersectedGameObjects;
}


svengine::Subject* svengine::RectCollider2DComponent::GetCollisionEnterEvent() const
{
	return m_CollisionEnterEvent.get();
}

svengine::Subject* svengine::RectCollider2DComponent::GetCollisionExitEvent() const
{
	return m_CollisionExitEvent.get();
}

svengine::Subject* svengine::RectCollider2DComponent::GetCollisionStayEvent() const
{
	return m_CollisionStayEvent.get();
}

svengine::RectCollider2DComponent::Rect svengine::RectCollider2DComponent::GetCollisionRect() const
{
	return m_CollisionRect;
}

void svengine::RectCollider2DComponent::RemoveAllCollidersFromScene(const Scene* scene)
{
	auto it = m_Colliders.begin();
	while (it != m_Colliders.end())
	{
		if ((*it)->GetOwner()->GetParentScene() == scene)
		{
			it = m_Colliders.erase(it);
		}
		else
		{
			++it;
		}
	}
}
