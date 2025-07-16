
#include "GameObject.h"
#include "ResourceManager.h"

svengine::GameObject::~GameObject() = default;

void svengine::GameObject::Start()
{
	for (const auto& component : m_Components)
	{
		if (component->IsActive())
		{
			component->Start();
		}
	}
}

void svengine::GameObject::Update(float elapsedSec)
{
	if (!m_RunsUpdate)
	{
		return;
	}

	for (const auto& component : m_Components)
	{
		if (component->IsActive())
		{
			component->Update(elapsedSec);
		}
	}
}

void svengine::GameObject::LateUpdate(float elapsedSec)
{
	if (m_RunsUpdate)
	{
		for (const auto& component : m_Components)
		{
			if (component->IsActive())
			{
				component->LateUpdate(elapsedSec);
			}
		}
	}

	// Deletion of marked objects happens at the very end

	auto rangeToDestroy = std::ranges::remove_if(m_Components, [](const auto& component)
		{
			return component->IsMarkedToDestroy();
		});

	m_Components.erase(rangeToDestroy.begin(), m_Components.end());
}


void svengine::GameObject::Render() const
{
	for (const auto& component : m_Components)
	{
		if (component->IsActive())
		{
			component->Render();
		}
	}
}

void svengine::GameObject::SetLocalPosition(float x, float y)
{
	m_LocalTransform.SetPosition(x, y, 0.0f);
	SetPositionIsDirty();
}

void svengine::GameObject::SetLocalPosition(const glm::vec3& pos)
{
	m_LocalTransform.SetPosition(pos);
	SetPositionIsDirty();
}

void svengine::GameObject::SetWorldPosition(float x, float y)
{
	m_WorldTransform.SetPosition(x, y, 0.0f);

	if (m_Parent == nullptr)
	{
		m_LocalTransform.SetPosition(x, y, 0.0f);
	}
	else
	{
		m_LocalTransform.SetPosition(glm::vec3{x, y, 0.0f} - m_Parent->GetWorldTransform().GetPosition());
	}

	for (const auto child : m_Children)
	{
		child->SetPositionIsDirty();
	}
}

void svengine::GameObject::SetWorldPosition(const glm::vec3& pos)
{
	m_WorldTransform.SetPosition(pos);

	if (m_Parent == nullptr)
	{
		m_LocalTransform.SetPosition(pos);
	}
	else
	{
		m_LocalTransform.SetPosition(pos - m_Parent->GetWorldTransform().GetPosition());
	}

	for (const auto child : m_Children)
	{
		child->SetPositionIsDirty();
	}
}

void svengine::GameObject::SetIsActive(bool newIsActive)
{
	m_IsActive = newIsActive;

	for (const auto child : m_Children)
	{
		child->SetIsActive(newIsActive);
	}
}

bool svengine::GameObject::IsActive() const
{
	return m_IsActive;
}

void svengine::GameObject::SetRunsUpdate(bool runsUpdate)
{
	m_RunsUpdate = runsUpdate;
}

const svengine::Transform& svengine::GameObject::GetWorldTransform()
{
	if (m_PositionIsDirty)
	{
		UpdateWorldPosition();
	}

	return m_WorldTransform;
}

const svengine::Transform& svengine::GameObject::GetLocalTransform() const
{
	return m_LocalTransform;
}


void svengine::GameObject::UpdateWorldPosition()
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

void svengine::GameObject::SetPositionIsDirty()
{
	m_PositionIsDirty = true;

	for (const auto child : m_Children)
	{
		child->SetPositionIsDirty();
	}
}

void svengine::GameObject::SetParent(GameObject* newParentPtr, bool worldPositionStays)
{
	// Validation
	assert((newParentPtr == nullptr && m_Parent == nullptr) || (newParentPtr != m_Parent && newParentPtr != this && IsNotInChildren(this)));

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
	if (m_Parent)
	{
		auto& childrenVec = m_Parent->GetChildrenVector();

		auto childToRemove = std::ranges::remove_if(childrenVec, [this](const auto& child)
			{
				return child == this;
			});

		childrenVec.erase(childToRemove.begin(), childrenVec.end());
	}

	// Setting the new parent
	m_Parent = newParentPtr;

	// Adding to the new parents children
	if (m_Parent)
	{
		auto& childrenVec = m_Parent->GetChildrenVector();

		childrenVec = m_Parent->GetChildrenVector();

		childrenVec.push_back(this);
	}
}

bool svengine::GameObject::IsNotInChildren(GameObject* gameObject) const
{
	return std::ranges::none_of(m_Children, [gameObject](const auto& child) 
		{
			return child == gameObject;
		});
}

std::vector<svengine::GameObject*>& svengine::GameObject::GetChildrenVector()
{
	return m_Children;
}

svengine::GameObject* svengine::GameObject::GetParent() const
{
	return m_Parent;
}

void svengine::GameObject::SetParentScene(Scene* scene)
{
	m_ParentScene = scene;
}

svengine::Scene* svengine::GameObject::GetParentScene() const
{
	return m_ParentScene;
}



void svengine::GameObject::Destroy()
{
	m_MarkedToDestroy = true;

	for (const auto child : m_Children)
	{
		assert(child != nullptr);

		child->Destroy();
	}
}

bool svengine::GameObject::IsMarkedToDestroy() const
{
	return m_MarkedToDestroy;
}

Tag svengine::GameObject::GetTag() const
{
	return m_Tag;
}

void svengine::GameObject::SetTag(Tag tag)
{
	m_Tag = tag;
}





