#include "LevelPartComponent.h"

dae::LevelPartComponent::LevelPartComponent(GameObject* ownerPtr, LevelPartType partType): ComponentBase(ownerPtr), m_PartType{partType}
{
}

void dae::LevelPartComponent::Update(float)
{
	
}

void dae::LevelPartComponent::LateUpdate(float)
{
	
}

void dae::LevelPartComponent::Render() const
{
	
}

void dae::LevelPartComponent::SetLocalPosition(float x, float y)
{
	m_LocalTransform.SetPosition(x, y, 0.0f);
}

dae::LevelPartComponent::LevelPartType dae::LevelPartComponent::GetLevelPartType() const
{
	return m_PartType;
}





