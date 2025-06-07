#include "HealthDisplayComponent.h"

#include <utility>

#include "GameObject.h"
#include "PeterPepperComponent.h"
#include "Scene.h"
#include "SceneManager.h"
#include "Texture2DComponent.h"

dae::HealthDisplayComponent::HealthDisplayComponent(GameObject* ownerPtr, std::vector<Texture2DComponent*> lifeDisplays) :
	ComponentBase(ownerPtr),
	m_LifeDisplays{std::move(lifeDisplays)}
{
}

void dae::HealthDisplayComponent::Start()
{
	auto players = SceneManager::GetInstance().GetDontDestroyOnLoadScene()->GetGameObjectsWithTag(make_sdbm_hash("Player"));
	players.front()->GetComponent<PeterPepperComponent>()->GetPlayerDiedEvent()->AddObserver(this);
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

void dae::HealthDisplayComponent::Notify(const Event& event, GameObject* observedGameObject)
{
	if (event.id == make_sdbm_hash("PlayerDied"))
	{
		const int remainingLives = observedGameObject->GetComponent<PeterPepperComponent>()->GetRemainingLives();

		if (remainingLives != 0)
		{
			m_LifeDisplays[remainingLives - 1]->SetIsActive(false);
		}
	}

	if (event.id == make_sdbm_hash("PlayerRespawned"))
	{
		for (auto lifeDisplay : m_LifeDisplays)
		{
			lifeDisplay->SetIsActive(true);
		}
	}
}





