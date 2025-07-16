#include "Texture2DComponent.h"

#include "Renderer.h"
#include "ResourceManager.h"
#include "GameObject.h"

svengine::Texture2DComponent::Texture2DComponent(GameObject* ownerPtr, const std::string& filename) :
	ComponentBase(ownerPtr),
	m_Texture(nullptr)
{
	m_Texture = ResourceManager::GetInstance().LoadTexture(filename);
}

void svengine::Texture2DComponent::Start()
{
}

void svengine::Texture2DComponent::Update(float)
{
}

void svengine::Texture2DComponent::LateUpdate(float)
{
}

void svengine::Texture2DComponent::Render() const
{
	const auto& pos = GetOwner()->GetWorldTransform().GetPosition() + GetLocalTransform().GetPosition();
	Renderer::GetInstance().RenderTexture(*m_Texture, pos.x, pos.y);
}


