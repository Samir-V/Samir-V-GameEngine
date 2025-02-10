#include "Texture2DComponent.h"

#include "Renderer.h"
#include "ResourceManager.h"

Texture2DComponent::Texture2DComponent(const std::string& filename) : m_Texture(nullptr)
{
	m_Texture = dae::ResourceManager::GetInstance().LoadTexture(filename);
}

void Texture2DComponent::Update(float elapsedSec)
{
	elapsedSec;
}

void Texture2DComponent::FixedUpdate(float fixedTimeStep)
{
	fixedTimeStep;
}

void Texture2DComponent::Render() const
{
	const auto& pos = m_Transform.GetPosition();
	dae::Renderer::GetInstance().RenderTexture(*m_Texture, pos.x, pos.y);
}

void Texture2DComponent::SetPosition(float x, float y)
{
	m_Transform.SetPosition(x, y, 0.0f);
}




