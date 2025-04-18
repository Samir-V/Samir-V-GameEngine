#include "BurgerPartComponent.h"

#include <memory>

#include "Renderer.h"
#include "ResourceManager.h"
#include "GameObject.h"
#include "RectCollider2DComponent.h"

dae::BurgerPartComponent::BurgerPartComponent(GameObject* ownerPtr, const std::string& filepath, int nrOfSlices): ComponentBase(ownerPtr), m_NrOfSlices{nrOfSlices}
{
	m_Texture = ResourceManager::GetInstance().LoadTexture(filepath);

	const auto& textureSize = m_Texture->GetSize();

	int sliceWidth = textureSize.x / m_NrOfSlices;

	m_SrcRects.assign(m_NrOfSlices, {});
	m_OffsetsY.assign(m_NrOfSlices, 0.0f);

	for (int index = 0; index < m_NrOfSlices; ++index)
	{
		m_SrcRects[index] = { (index * sliceWidth), 0, sliceWidth, textureSize.y };
	}
}

void dae::BurgerPartComponent::Update(float elapsedSec)
{
	if (m_BurgerPartState == BurgerPartState::Falling)
	{
		const auto& burgerPartPos = GetOwner()->GetWorldTransform().GetPosition();

		const auto newBurgerPartPosY = burgerPartPos.y + m_Velocity.y * elapsedSec;

		GetOwner()->SetLocalPosition(burgerPartPos.x, newBurgerPartPosY);
	}
}

void dae::BurgerPartComponent::LateUpdate(float)
{
}

void dae::BurgerPartComponent::Render() const
{
	const auto& pos = GetOwner()->GetWorldTransform().GetPosition();
	for (int index = 0; index < m_NrOfSlices; ++index)
	{
		Renderer::GetInstance().RenderTexture(*m_Texture, pos.x + index * m_SrcRects[index].w, pos.y + m_OffsetsY[index], &m_SrcRects[index]);
	}
}

void dae::BurgerPartComponent::SetLocalPosition(float x, float y)
{
	m_LocalTransform.SetPosition(x, y, 0.0f);
}

void dae::BurgerPartComponent::Notify(const Event& event, GameObject* observedGameObject)
{
	if (event.id == make_sdbm_hash("OnCollisionEnter"))
	{
		// Checking when falling if burger part collided with platform, to stop
		if (m_BurgerPartState == BurgerPartState::Falling && observedGameObject->GetTag() == make_sdbm_hash("Platform"))
		{
			const auto& burgerPartPos = GetOwner()->GetWorldTransform().GetPosition();

			const auto newBurgerPartPosY = observedGameObject->GetWorldTransform().GetPosition().y - 2.0f;

			GetOwner()->SetLocalPosition(burgerPartPos.x, newBurgerPartPosY);

			m_BurgerPartState = BurgerPartState::Idle;
			std::ranges::fill(m_OffsetsY, 0.0f);
		}

		// Checking when falling if burger part collided with another burger part, that is already in final position
		if (m_BurgerPartState == BurgerPartState::Falling && observedGameObject->GetTag() == make_sdbm_hash("BurgerPart"))
		{
			if (observedGameObject->GetComponent<BurgerPartComponent>()->m_BurgerPartState == BurgerPartState::Assembled)
			{
				const auto& burgerPartPos = GetOwner()->GetWorldTransform().GetPosition();

				const auto newBurgerPartPosY = observedGameObject->GetWorldTransform().GetPosition().y - 7.0f;

				GetOwner()->SetLocalPosition(burgerPartPos.x, newBurgerPartPosY);

				m_BurgerPartState = BurgerPartState::Assembled;
				std::ranges::fill(m_OffsetsY, 0.0f);
			}
		}


		// Checking if burger part was hit by another burger part
		if (m_BurgerPartState == BurgerPartState::Idle && observedGameObject->GetTag() == make_sdbm_hash("BurgerPart"))
		{
			m_BurgerPartState = BurgerPartState::Falling;
			std::ranges::fill(m_OffsetsY, 2.0f);
		}

		// Checking if burger part has hit final plate
		if (m_BurgerPartState == BurgerPartState::Falling && observedGameObject->GetTag() == make_sdbm_hash("Plate"))
		{
			const auto& burgerPartPos = GetOwner()->GetWorldTransform().GetPosition();

			const auto newBurgerPartPosY = observedGameObject->GetWorldTransform().GetPosition().y - 3.0f;

			GetOwner()->SetLocalPosition(burgerPartPos.x, newBurgerPartPosY);

			m_BurgerPartState = BurgerPartState::Assembled;
			std::ranges::fill(m_OffsetsY, 0.0f);
		}


		// Caching the PlayerCollider in the Collision Start, to not access it every frame in Collision Stay

		if (observedGameObject->GetTag() == make_sdbm_hash("Player"))
		{
			m_PlayerCollider = observedGameObject->GetComponent<RectCollider2DComponent>();
		}
	}

	if (event.id == make_sdbm_hash("OnCollisionStay"))
	{
		if (m_BurgerPartState == BurgerPartState::Idle && observedGameObject->GetTag() == make_sdbm_hash("Player"))
		{
			const auto playerColliderRect = m_PlayerCollider->GetCollisionRect();

			const auto& pos = GetOwner()->GetWorldTransform().GetPosition();
			const int sliceWidth = m_SrcRects[0].w;

			const float playerCenterX = playerColliderRect.posX + playerColliderRect.width * 0.5f;

			int idx = static_cast<int>(playerCenterX - pos.x) / sliceWidth;
			idx = std::clamp(idx, 0, static_cast<int>(m_SrcRects.size()) - 1);

			m_OffsetsY[idx] = 2.0f;
		}

		if (m_BurgerPartState == BurgerPartState::Idle && observedGameObject->GetTag() == make_sdbm_hash("Player"))
		{
			const bool everyPartHasOffset = std::ranges::all_of(m_OffsetsY, [](float offset)
				{
					return offset > 0.0f;
				});

			if (everyPartHasOffset)
			{
				m_BurgerPartState = BurgerPartState::Falling;
			}
		}
	}

	if (event.id == make_sdbm_hash("OnCollisionExit"))
	{

	}
}




