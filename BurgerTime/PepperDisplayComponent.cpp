#include "PepperDisplayComponent.h"

#include "PeterPepperComponent.h"
#include "Scene.h"
#include "SceneManager.h"
#include "TextComponent.h"

dae::PepperDisplayComponent::PepperDisplayComponent(GameObject* ownerPtr, TextComponent* textComponent, GameObject* petterPepper):
	ComponentBase(ownerPtr)
	, m_PepperDisplay{textComponent}
	, m_PeterPepperPtr{ petterPepper }
{
}

void dae::PepperDisplayComponent::Start()
{
	assert(m_PeterPepperPtr != nullptr);

	m_PeterPepperPtr->GetComponent<PeterPepperComponent>()->GetPepperAmountChangedEvent()->AddObserver(this);
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
	if (event.id == make_sdbm_hash("PepperAmountChanged"))
	{
		int remainingPeppers = observedGameObject->GetComponent<PeterPepperComponent>()->GetRemainingPeppers();

		m_PepperDisplay->SetText("Peppers: " + std::to_string(remainingPeppers));
	}
}




