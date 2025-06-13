#include "HealthDisplayComponent.h"

#include <utility>

#include "GameObject.h"
#include "PeterPepperComponent.h"
#include "SceneManager.h"
#include "Texture2DComponent.h"

dae::HealthDisplayComponent::HealthDisplayComponent(GameObject* ownerPtr, std::vector<Texture2DComponent*> lifeDisplays, GameObject* peterPepper) :
	ComponentBase(ownerPtr)
	, m_LifeDisplays{std::move(lifeDisplays)}
	, m_PeterPepperPtr{peterPepper}
{
}

void dae::HealthDisplayComponent::Start()
{
	assert(m_PeterPepperPtr != nullptr);

	m_PeterPepperPtr->GetComponent<PeterPepperComponent>()->GetPlayerDiedEvent()->AddObserver(this);
}

void dae::HealthDisplayComponent::Update(float)
{
}

void dae::HealthDisplayComponent::LateUpdate(float)
{
}

void dae::HealthDisplayComponent::Render() const
{
}

void dae::HealthDisplayComponent::Notify(const Event& event, GameObject* gameObjectCausingEvent)
{
	if (event.id == make_sdbm_hash("PlayerDied"))
	{
		const int remainingLives = gameObjectCausingEvent->GetComponent<PeterPepperComponent>()->GetRemainingLives();

		const int index = remainingLives - 1;

		if (index >= 0 && index < static_cast<int>(m_LifeDisplays.size()))
		{
			m_LifeDisplays[index]->SetIsActive(false);
		}
	}

	if (event.id == make_sdbm_hash("PlayerRespawned"))
	{
		for (const auto lifeDisplay : m_LifeDisplays)
		{
			lifeDisplay->SetIsActive(true);
		}
	}
}





