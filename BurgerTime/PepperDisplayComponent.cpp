#include "PepperDisplayComponent.h"

#include "PeterPepperComponent.h"
#include "Scene.h"
#include "SceneManager.h"
#include "TextComponent.h"

dae::PepperDisplayComponent::PepperDisplayComponent(GameObject* ownerPtr, TextComponent* textComponent): ComponentBase(ownerPtr), m_PepperDisplay{textComponent}
{
}

void dae::PepperDisplayComponent::Start()
{
	auto players = SceneManager::GetInstance().GetDontDestroyOnLoadScene()->GetGameObjectsWithTag(make_sdbm_hash("Player"));

	players.front()->GetComponent<PeterPepperComponent>()->GetPepperSprayedEvent()->AddObserver(this);
}

void dae::PepperDisplayComponent::Update(float)
{
}

void dae::PepperDisplayComponent::LateUpdate(float)
{
}

void dae::PepperDisplayComponent::Render() const
{
}

void dae::PepperDisplayComponent::Notify(const Event& event, GameObject* observedGameObject)
{
	if (event.id == make_sdbm_hash("PepperSprayed"))
	{
		int remainingPeppers = observedGameObject->GetComponent<PeterPepperComponent>()->GetRemainingPeppers();

		m_PepperDisplay->SetText("Peppers: " + std::to_string(remainingPeppers));
	}

	if (event.id == make_sdbm_hash("PlayerRespawned"))
	{
		int remainingPeppers = observedGameObject->GetComponent<PeterPepperComponent>()->GetRemainingPeppers();

		m_PepperDisplay->SetText("Peppers: " + std::to_string(remainingPeppers));
	}
}




