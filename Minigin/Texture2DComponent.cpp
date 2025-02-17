#include "Texture2DComponent.h"

#include "Renderer.h"
#include "ResourceManager.h"

dae::Texture2DComponent::Texture2DComponent(GameObject* ownerPtr, const std::string& filename) :
	ComponentBase(ownerPtr),
	m_Texture(nullptr)
{
	m_Texture = ResourceManager::GetInstance().LoadTexture(filename);
}

void dae::Texture2DComponent::Update(float)
{

}

void dae::Texture2DComponent::FixedUpdate(float)
{

}

void dae::Texture2DComponent::LateUpdate(float)
{

}

void dae::Texture2DComponent::Render() const
{
	const auto& pos = m_Transform.GetPosition();
	Renderer::GetInstance().RenderTexture(*m_Texture, pos.x, pos.y);
}

void dae::Texture2DComponent::SetPosition(float x, float y)
{
	m_Transform.SetPosition(x, y, 0.0f);
}

void dae::Texture2DComponent::Destroy()
{
	m_MarkedToDestroy = true;
}

bool dae::Texture2DComponent::GetMarkedToDestroy() const
{
	return m_MarkedToDestroy;
}




