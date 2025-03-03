#include "Texture2DComponent.h"

#include "Renderer.h"
#include "ResourceManager.h"
#include "GameObject.h"

dae::Texture2DComponent::Texture2DComponent(GameObject* ownerPtr, const std::string& filename) :
	ComponentBase(ownerPtr),
	m_Texture(nullptr)
{
	m_Texture = ResourceManager::GetInstance().LoadTexture(filename);
}

void dae::Texture2DComponent::Update(float)
{

}

void dae::Texture2DComponent::LateUpdate(float)
{

}

void dae::Texture2DComponent::Render() const
{
	const auto& pos = GetOwner()->GetWorldTransform().GetPosition() + m_LocalTransform.GetPosition();
	Renderer::GetInstance().RenderTexture(*m_Texture, pos.x, pos.y);
}

void dae::Texture2DComponent::RenderImGui()
{
	
}


void dae::Texture2DComponent::SetLocalPosition(float x, float y)
{
	m_LocalTransform.SetPosition(x, y, 0.0f);
}



