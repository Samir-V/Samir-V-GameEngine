#include "SpritesheetComponent.h"

#include "Renderer.h"
#include "ResourceManager.h"
#include "GameObject.h"

dae::SpritesheetComponent::SpritesheetComponent(GameObject* ownerPtr, std::string folderPath):
	ComponentBase(ownerPtr)
	, m_CurrentCol{0}
	, m_CurrentRow{0}
	, m_Accumulator{0}
	, m_FolderPath(std::move(folderPath))
{
}

void dae::SpritesheetComponent::Start()
{
}

void dae::SpritesheetComponent::Update(float elapsedSec)
{
	assert(m_CurrentSprite != nullptr);

	auto [numOfCols, numOfRows, timeStep] = m_CurrentSprite->metaData;

	if (numOfCols <= 1 && numOfRows == 0)
	{
		return;
	}

	m_Accumulator += elapsedSec;

	if (m_Accumulator < timeStep)
	{
		return;
	}

	m_Accumulator = 0.0f;
	m_CurrentCol++;

	if (m_CurrentCol < numOfCols)
	{
		return;
	}

	m_CurrentCol = 0;
	m_CurrentRow++;

	if (m_CurrentRow < numOfRows)
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
	const auto& pos = GetOwner()->GetWorldTransform().GetPosition() + GetLocalTransform().GetPosition();

	const auto textureSize = m_CurrentSprite->texture->GetSize();
	const auto metaData = m_CurrentSprite->metaData;

	const auto frameWidth = textureSize.x / metaData.numOfCols;
	auto frameHeight = textureSize.y;

	SDL_Rect sourceRect{};

	sourceRect.x = frameWidth * m_CurrentCol;

	if (metaData.numOfRows != 0)
	{
		frameHeight = textureSize.y / metaData.numOfRows;
		sourceRect.y = frameHeight * m_CurrentRow;
	}

	sourceRect.w = frameWidth;
	sourceRect.h = frameHeight;

	Renderer::GetInstance().RenderTexture(*m_CurrentSprite->texture, pos.x, pos.y, &sourceRect);
}

void dae::SpritesheetComponent::AddSprite(const std::string& spriteName, SpriteID spriteId, SpriteMetaData spriteMetadata)
{
	std::string fullPath{};

	if (!m_FolderPath.empty())
	{
		fullPath = m_FolderPath + "/" + spriteName;
	}
	else
	{
		fullPath = spriteName;
	}

	const auto texture = ResourceManager::GetInstance().LoadTexture(fullPath);
	m_SpriteSheet.insert({ spriteId, SpriteData{ texture, spriteMetadata }});
}

void dae::SpritesheetComponent::Play(SpriteID spriteId)
{
	auto& newSprite = m_SpriteSheet.at(spriteId);

	if (m_CurrentSprite == nullptr || newSprite.texture != m_CurrentSprite->texture)
	{
		m_CurrentCol = 0;
		m_CurrentRow = 0;
		m_Accumulator = 0.0f;
		m_CurrentSprite = &newSprite;
	}
}

void dae::SpritesheetComponent::ResetSpriteTiming()
{
	m_CurrentCol = 0;
	m_CurrentRow = 0;
	m_Accumulator = 0.0f;
}





