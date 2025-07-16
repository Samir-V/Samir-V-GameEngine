#include "PepperDisplayComponent.h"

#include "PeterPepperComponent.h"
#include "Scene.h"
#include "SceneManager.h"
#include "TextComponent.h"

svengine::PepperDisplayComponent::PepperDisplayComponent(GameObject* ownerPtr, TextComponent* textComponent, GameObject* peterPepper):
	ComponentBase(ownerPtr)
	, m_PepperDisplay{textComponent}
	, m_PeterPepperPtr{ peterPepper }
{
}

void svengine::PepperDisplayComponent::Start()
{
	assert(m_PeterPepperPtr != nullptr);

	m_PeterPepperPtr->GetComponent<PeterPepperComponent>()->GetPepperAmountChangedEvent()->AddObserver(this);
}

void svengine::PepperDisplayComponent::Update(float)
{
}

void svengine::PepperDisplayComponent::LateUpdate(float)
{
}

void svengine::PepperDisplayComponent::Render() const
{
}

void svengine::PepperDisplayComponent::Notify(const Event& event, GameObject* gameObjectCausingEvent)
{
	if (event.id == make_sdbm_hash("PepperAmountChanged"))
	{
		const int remainingPeppers = gameObjectCausingEvent->GetComponent<PeterPepperComponent>()->GetRemainingPeppers();

		m_PepperDisplay->SetText("Peppers: " + std::to_string(remainingPeppers));
	}
}




