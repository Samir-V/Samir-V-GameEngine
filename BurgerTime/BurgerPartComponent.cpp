#include "BurgerPartComponent.h"

#include <memory>

#include "EnemyComponent.h"
#include "Renderer.h"
#include "ResourceManager.h"
#include "GameObject.h"
#include "RectCollider2DComponent.h"
#include "ServiceLocator.h"

svengine::BurgerPartComponent::BurgerPartComponent(GameObject* ownerPtr, const std::string& filepath, int nrOfSlices):
	ComponentBase(ownerPtr)
	, m_NrOfSlices{nrOfSlices}
	, m_ExtraLevelsToFall{0}
	, m_BurgerPartCollisionEvent{ std::make_unique<Subject>() }
{
	m_SrcRects.assign(m_NrOfSlices, {});
	m_OffsetsY.assign(m_NrOfSlices, 0.0f);

	m_Texture = ResourceManager::GetInstance().LoadTexture(filepath);
	const auto& textureSize = m_Texture->GetSize();
	const int sliceWidth = textureSize.x / m_NrOfSlices;

	for (int index = 0; index < m_NrOfSlices; ++index)
	{
		m_SrcRects[index] = { (index * sliceWidth), 0, sliceWidth, textureSize.y };
	}
}

void svengine::BurgerPartComponent::Start()
{
	const auto rectColliderComp = GetOwner()->GetComponent<RectCollider2DComponent>();

	rectColliderComp->GetCollisionEnterEvent()->AddObserver(this);
	rectColliderComp->GetCollisionStayEvent()->AddObserver(this);
	rectColliderComp->GetCollisionExitEvent()->AddObserver(this);
}

void svengine::BurgerPartComponent::Update(float elapsedSec)
{
	std::erase_if(m_EnemiesOnTop, [](const GameObject* gameObject) {
		return (gameObject == nullptr) || (gameObject->IsMarkedToDestroy()) || (!gameObject->IsActive());
		});

	if (m_BurgerPartState == BurgerPartState::Falling)
	{
		const auto& burgerPartPos = GetOwner()->GetWorldTransform().GetPosition();

		const auto newBurgerPartPosY = burgerPartPos.y + m_Velocity.y * elapsedSec;

		GetOwner()->SetWorldPosition(burgerPartPos.x, newBurgerPartPosY);
	}
}

void svengine::BurgerPartComponent::LateUpdate(float)
{
}

void svengine::BurgerPartComponent::Render() const
{
	const auto& pos = GetOwner()->GetWorldTransform().GetPosition() + GetLocalTransform().GetPosition();
	for (int index = 0; index < m_NrOfSlices; ++index)
	{
		Renderer::GetInstance().RenderTexture(*m_Texture, pos.x + index * m_SrcRects[index].w, pos.y + m_OffsetsY[index], &m_SrcRects[index]);
	}
}

void svengine::BurgerPartComponent::Notify(const Event& event, GameObject* gameObjectCausingEvent)
{
	auto& soundSystem = ServiceLocator::GetSoundSystem();

	if (event.id == make_sdbm_hash("OnCollisionEnter"))
	{
		// Checking if burger part was hit by another burger part
		if (m_BurgerPartState == BurgerPartState::Idle && gameObjectCausingEvent->GetTag() == make_sdbm_hash("BurgerPart"))
		{
			m_BurgerPartState = BurgerPartState::Falling;
			std::ranges::fill(m_OffsetsY, 2.0f);

			soundSystem.Play("BurgerFall.wav", 0.5f);
		}

		// Keep track of the enemies on top of the burger bun
		if (m_BurgerPartState == BurgerPartState::Idle && gameObjectCausingEvent->GetTag() == make_sdbm_hash("Enemy"))
		{
			m_EnemiesOnTop.push_back(gameObjectCausingEvent);
		}

		// Checking when falling if burger part collided with platform, to stop
		if (m_BurgerPartState == BurgerPartState::Falling && gameObjectCausingEvent->GetTag() == make_sdbm_hash("Platform"))
		{
			if (m_ExtraLevelsToFall > 0)
			{
				--m_ExtraLevelsToFall;

				soundSystem.Play("BurgerLand.wav", 0.5f);
				m_BurgerPartCollisionEvent->NotifyObservers(Event(make_sdbm_hash("BurgerPartLanded")), GetOwner());
			}
			else
			{
				const auto& burgerPartPos = GetOwner()->GetWorldTransform().GetPosition();

				const auto newBurgerPartPosY = gameObjectCausingEvent->GetWorldTransform().GetPosition().y - 2.0f;

				GetOwner()->SetWorldPosition(burgerPartPos.x, newBurgerPartPosY);

				m_BurgerPartState = BurgerPartState::Idle;
				std::ranges::fill(m_OffsetsY, 0.0f);

				soundSystem.Play("BurgerLand.wav", 0.5f);
				m_BurgerPartCollisionEvent->NotifyObservers(Event(make_sdbm_hash("BurgerPartLanded")), GetOwner());
			}
		}

		// Checking when falling if burger part collided with another burger part, that is already in final position
		if (m_BurgerPartState == BurgerPartState::Falling && gameObjectCausingEvent->GetTag() == make_sdbm_hash("BurgerPart"))
		{
			if (gameObjectCausingEvent->GetComponent<BurgerPartComponent>()->m_BurgerPartState == BurgerPartState::Assembled)
			{
				// Just to be sure
				m_ExtraLevelsToFall = 0;

				const auto& burgerPartPos = GetOwner()->GetWorldTransform().GetPosition();

				const auto newBurgerPartPosY = gameObjectCausingEvent->GetWorldTransform().GetPosition().y - 7.0f;

				GetOwner()->SetWorldPosition(burgerPartPos.x, newBurgerPartPosY);

				m_BurgerPartState = BurgerPartState::Assembled;
				std::ranges::fill(m_OffsetsY, 0.0f);

				soundSystem.Play("BurgerLand.wav", 0.5f);
				m_BurgerPartCollisionEvent->NotifyObservers(Event(make_sdbm_hash("BurgerPartLanded")), GetOwner());
			}
		}

		// Checking if burger part has hit final plate
		if (m_BurgerPartState == BurgerPartState::Falling && gameObjectCausingEvent->GetTag() == make_sdbm_hash("Plate"))
		{
			// Just to be sure
			m_ExtraLevelsToFall = 0;

			const auto& burgerPartPos = GetOwner()->GetWorldTransform().GetPosition();

			const auto newBurgerPartPosY = gameObjectCausingEvent->GetWorldTransform().GetPosition().y - 3.0f;

			GetOwner()->SetWorldPosition(burgerPartPos.x, newBurgerPartPosY);

			m_BurgerPartState = BurgerPartState::Assembled;
			std::ranges::fill(m_OffsetsY, 0.0f);

			soundSystem.Play("BurgerLand.wav", 0.5f);
			m_BurgerPartCollisionEvent->NotifyObservers(Event(make_sdbm_hash("BurgerPartLanded")), GetOwner());
		}


		// Caching the PlayerCollider in the Collision Start, to not access it every frame in Collision Stay

		if (gameObjectCausingEvent->GetTag() == make_sdbm_hash("Player"))
		{
			m_PlayerCollider = gameObjectCausingEvent->GetComponent<RectCollider2DComponent>();
		}
	}

	if (event.id == make_sdbm_hash("OnCollisionStay"))
	{
		if (m_BurgerPartState == BurgerPartState::Idle && gameObjectCausingEvent->GetTag() == make_sdbm_hash("Player"))
		{
			const auto playerColliderRect = m_PlayerCollider->GetCollisionRect();

			const auto& pos = GetOwner()->GetWorldTransform().GetPosition();

			const float playerCenterX = playerColliderRect.posX + playerColliderRect.width * 0.5f;
			const float playerCenterY = playerColliderRect.posY + playerColliderRect.height * 0.5f;

			if (const float burgerY = pos.y; playerCenterY < burgerY)
			{
				const int sliceWidth = m_SrcRects[0].w;

				int idx = static_cast<int>(playerCenterX - pos.x) / sliceWidth;
				idx = std::clamp(idx, 0, static_cast<int>(m_SrcRects.size()) - 1);

				if (m_OffsetsY[idx] < 2.0f)
				{
					m_OffsetsY[idx] = 2.0f;
					soundSystem.Play("BurgerStep.wav", 0.3f);
				}
			}
		}

		if (m_BurgerPartState == BurgerPartState::Idle && gameObjectCausingEvent->GetTag() == make_sdbm_hash("Player"))
		{
			const bool everyPartHasOffset = std::ranges::all_of(m_OffsetsY, [](float offset)
				{
					return offset > 0.0f;
				});

			if (everyPartHasOffset)
			{

				std::set<EnemyComponent*> actualEnemiesToFall{};
				constexpr float offset{ 2.0f };

				for (const auto enemy : m_EnemiesOnTop)
				{
					const auto ownerYPos = GetOwner()->GetWorldTransform().GetPosition().y;
					const auto enemyYPos = enemy->GetWorldTransform().GetPosition().y;

					if (enemyYPos < ownerYPos + offset)
					{
						const auto enemyComponent = enemy->GetComponent<EnemyComponent>();
						enemyComponent->StartFalling(GetOwner());
						++m_ExtraLevelsToFall;
					}
				}

				if (m_ExtraLevelsToFall != 0)
				{
					switch (m_ExtraLevelsToFall)
					{
					case 1:
						m_BurgerPartCollisionEvent->NotifyObservers(Event(make_sdbm_hash("EnemyTop1")), GetOwner());
						break;
					case 2:
						m_BurgerPartCollisionEvent->NotifyObservers(Event(make_sdbm_hash("EnemyTop2")), GetOwner());
						break;
					case 3:
						m_BurgerPartCollisionEvent->NotifyObservers(Event(make_sdbm_hash("EnemyTop3")), GetOwner());
						break;
					case 4:
						m_BurgerPartCollisionEvent->NotifyObservers(Event(make_sdbm_hash("EnemyTop4")), GetOwner());
						break;
					case 5:
						m_BurgerPartCollisionEvent->NotifyObservers(Event(make_sdbm_hash("EnemyTop5")), GetOwner());
						break;
					case 6:
						m_BurgerPartCollisionEvent->NotifyObservers(Event(make_sdbm_hash("EnemyTop6")), GetOwner());
						break;
					default: 
						break;
					}
				}

				m_EnemiesOnTop.clear();
			
				m_BurgerPartState = BurgerPartState::Falling;
				soundSystem.Play("BurgerFall.wav", 0.5f);
			}
		}

		// Checking when falling if burger part collided with another burger part, that is already in final position (When multiple parts fall at the same time)
		if (m_BurgerPartState == BurgerPartState::Falling && gameObjectCausingEvent->GetTag() == make_sdbm_hash("BurgerPart"))
		{
			if (gameObjectCausingEvent->GetComponent<BurgerPartComponent>()->m_BurgerPartState == BurgerPartState::Assembled)
			{
				m_ExtraLevelsToFall = 0;

				const auto& burgerPartPos = GetOwner()->GetWorldTransform().GetPosition();

				const auto newBurgerPartPosY = gameObjectCausingEvent->GetWorldTransform().GetPosition().y - 7.0f;

				GetOwner()->SetWorldPosition(burgerPartPos.x, newBurgerPartPosY);

				m_BurgerPartState = BurgerPartState::Assembled;
				std::ranges::fill(m_OffsetsY, 0.0f);

				soundSystem.Play("BurgerLand.wav", 0.5f);
				m_BurgerPartCollisionEvent->NotifyObservers(Event(make_sdbm_hash("BurgerPartLanded")), GetOwner());
			}
		}
	}

	if (event.id == make_sdbm_hash("OnCollisionExit"))
	{
		if (gameObjectCausingEvent->GetTag() == make_sdbm_hash("Enemy"))
		{
			const auto it = std::ranges::find(m_EnemiesOnTop, gameObjectCausingEvent);
			if (it != m_EnemiesOnTop.end())
			{
				m_EnemiesOnTop.erase(it);
			}
		}
	}
}


svengine::Subject* svengine::BurgerPartComponent::GetBurgerPartCollisionEvent() const
{
	return m_BurgerPartCollisionEvent.get();
}

svengine::BurgerPartComponent::BurgerPartState svengine::BurgerPartComponent::GetBurgerPartState() const
{
	return m_BurgerPartState;
}




