#include "HealthDisplayComponent.h"

#include "GameObject.h"
#include "PeterPepperComponent.h"
#include "TextComponent.h"

dae::HealthDisplayComponent::HealthDisplayComponent(GameObject* ownerPtr, TextComponent* textComponent) :
	ComponentBase(ownerPtr),
	m_HealthDisplay(textComponent)
{

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

void dae::HealthDisplayComponent::SetLocalPosition(float x, float y)
{
	m_LocalTransform.SetPosition(x, y, 0.0f);
}

void dae::HealthDisplayComponent::Notify(const Event& event, GameObject* observedGameObject)
{
	if (event.id == make_sdbm_hash("PlayerDamaged"))
	{
		m_HealthDisplay->SetText("Remaining Health: " + std::to_string(observedGameObject->GetComponent<PeterPepperComponent>()->GetRemainingHealth()));
	}

	//switch (event)
	//{
	//case EventType::PlayerDamaged:

	//	m_HealthDisplay->SetText("Remaining Health: " + std::to_string(observedGameObject->GetComponent<PeterPepperComponent>()->GetRemainingHealth()));

	//	break;
	//case EventType::SubjectDestroyed:

	//	// Will handle destruction of the subject

	//	break;
	//}
}





