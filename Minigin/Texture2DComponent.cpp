#include "Texture2DComponent.h"

#include "Renderer.h"
#include "ResourceManager.h"

dae::Texture2DComponent::Texture2DComponent(const std::string& filename) : m_Texture(nullptr)
{
	m_Texture = dae::ResourceManager::GetInstance().LoadTexture(filename);
}

void dae::Texture2DComponent::Update(float elapsedSec)
{
	elapsedSec;
}

void dae::Texture2DComponent::FixedUpdate(float fixedTimeStep)
{
	fixedTimeStep;
}

void dae::Texture2DComponent::Render() const
{
	const auto& pos = m_Transform.GetPosition();
	dae::Renderer::GetInstance().RenderTexture(*m_Texture, pos.x, pos.y);
}

void dae::Texture2DComponent::SetPosition(float x, float y)
{
	m_Transform.SetPosition(x, y, 0.0f);
}

void dae::Texture2DComponent::SetOwner(std::weak_ptr<dae::GameObject> owner)
{
	m_Owner = owner;
}


