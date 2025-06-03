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

void dae::Texture2DComponent::Start()
{
}

void dae::Texture2DComponent::Update(float)
{
}

void dae::Texture2DComponent::LateUpdate(float)
{
}

void dae::Texture2DComponent::Render() const
{
	const auto& pos = GetOwner()->GetWorldTransform().GetPosition() + GetLocalTransform().GetPosition();
	Renderer::GetInstance().RenderTexture(*m_Texture, pos.x, pos.y);
}


