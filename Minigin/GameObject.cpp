
#include "GameObject.h"
#include "ResourceManager.h"

dae::GameObject::~GameObject() = default;

void dae::GameObject::Update(float elapsedSec)
{ 
	for (const auto& component : m_Components)
	{
		component->Update(elapsedSec);
	}
}

void dae::GameObject::LateUpdate(float elapsedSec)
{
	for (const auto& component : m_Components)
	{
		component->LateUpdate(elapsedSec);
	}

	// Deletion of marked objects happens at the very end

	auto rangeToDestroy = std::ranges::remove_if(m_Components, [](const auto& component)
		{
			return component->IsMarkedToDestroy();
		});

	m_Components.erase(rangeToDestroy.begin(), m_Components.end());
}


void dae::GameObject::Render() const
{
	for (const auto& component : m_Components)
	{
		component->Render();
	}
}

void dae::GameObject::SetLocalPosition(float x, float y)
{
	m_LocalTransform.SetPosition(x, y, 0.0f);
	SetPositionIsDirty();
}

void dae::GameObject::SetLocalPosition(const glm::vec3& pos)
{
	m_LocalTransform.SetPosition(pos);
	SetPositionIsDirty();
}


dae::Transform dae::GameObject::GetWorldTransform()
{
	if (m_PositionIsDirty)
	{
		UpdateWorldPosition();
	}
	return m_WorldTransform;
}

void dae::GameObject::UpdateWorldPosition()
{
	if (m_PositionIsDirty)
	{
		if (m_Parent == nullptr)
		{
			m_WorldTransform.SetPosition(m_LocalTransform.GetPosition());
		}
		else
		{
			m_WorldTransform.SetPosition(m_Parent->GetWorldTransform().GetPosition() + m_LocalTransform.GetPosition());
		}
	}
	m_PositionIsDirty = false;
}

void dae::GameObject::SetPositionIsDirty()
{
	m_PositionIsDirty = true;
}

void dae::GameObject::SetParent(GameObject* newParentPtr, bool worldPositionStays)
{
	// Validation
	assert(newParentPtr != m_Parent && newParentPtr != this && IsNotInChildren(this));

	if (newParentPtr == nullptr)
	{
		SetLocalPosition(GetWorldTransform().GetPosition());
	}
	else
	{
		if (worldPositionStays)
		{
			SetLocalPosition(GetWorldTransform().GetPosition() - m_Parent->GetWorldTransform().GetPosition());
		}
		SetPositionIsDirty();
	}

	// Removing from previous parent
	auto& childrenVec = m_Parent->GetChildrenVector();

	auto it = std::ranges::find_if(childrenVec, [this](const auto& child)
		{
			return child == this;
		});

	auto child = *it;
		
	childrenVec.erase(it);

	// Setting the new parent
	m_Parent = newParentPtr;

	// Adding to the new parents children
	childrenVec = m_Parent->GetChildrenVector();

	childrenVec.push_back(child);
}

bool dae::GameObject::IsNotInChildren(GameObject* gameObject) const
{
	return std::ranges::none_of(m_Children, [gameObject](const auto& child) 
		{
			return child == gameObject;
		});
}

std::vector<dae::GameObject*>& dae::GameObject::GetChildrenVector()
{
	return m_Children;
}
