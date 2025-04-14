#include "SpritesheetComponent.h"

#include "Renderer.h"
#include "ResourceManager.h"
#include "GameObject.h"

dae::SpritesheetComponent::SpritesheetComponent(GameObject* ownerPtr): ComponentBase(ownerPtr)
{
}

void dae::SpritesheetComponent::Update(float elapsedSec)
{
	assert(m_CurrentSprite != nullptr);

	m_Accumulator += elapsedSec;

	auto metaData = m_CurrentSprite->metaData;

	if (m_Accumulator < metaData.timeStep)
	{
		return;
	}

	m_Accumulator = 0.0f;
	m_CurrentCol++;

	if (m_CurrentCol < metaData.numOfCols)
	{
		return;
	}

	m_CurrentCol = 0;
	m_CurrentRow++;

	if (m_CurrentRow < metaData.numOfRows)
	{
		return;
	}

	m_CurrentRow = 0;
}

void dae::SpritesheetComponent::LateUpdate(float)
{
	
}

void dae::SpritesheetComponent::Render() const
{
	const auto& pos = GetOwner()->GetWorldTransform().GetPosition() + m_LocalTransform.GetPosition();

	auto textureSize = m_CurrentSprite->texture->GetSize();
	auto metaData = m_CurrentSprite->metaData;

	auto frameWidth = textureSize.x / metaData.numOfCols;
	auto frameHeight = textureSize.y / metaData.numOfRows;

	SDL_Rect sourceRect{};

	sourceRect.x = frameWidth * m_CurrentCol;

	if (metaData.numOfRows != 0)
	{
		sourceRect.y = frameHeight * m_CurrentRow;
	}

	sourceRect.w = textureSize.x / metaData.numOfCols;
	sourceRect.h = textureSize.y / metaData.numOfRows;

	Renderer::GetInstance().RenderTexture(*m_CurrentSprite->texture, pos.x, pos.y, &sourceRect);
}

void dae::SpritesheetComponent::SetLocalPosition(float x, float y)
{
	m_LocalTransform.SetPosition(x, y, 0.0f);
}

void dae::SpritesheetComponent::AddSprite(const std::string& spriteName, SpriteMetaData spriteMetadata)
{
	auto texture = ResourceManager::GetInstance().LoadTexture(spriteName);
	m_SpriteSheet.insert({spriteName, SpriteData{ texture, spriteMetadata }});
}

void dae::SpritesheetComponent::Play(const std::string& spriteName)
{
	if (m_SpriteSheet.at(spriteName).texture == m_CurrentSprite->texture)
	{
		return;
	}

	m_CurrentCol = 0;
	m_CurrentRow = 0;
	m_Accumulator = 0.0f;

	m_CurrentSprite = &m_SpriteSheet.at(spriteName);
}




