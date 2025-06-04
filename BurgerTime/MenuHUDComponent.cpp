#include "MenuHUDComponent.h"

#include <utility>

#include "Scene.h"
#include "SceneManager.h"
#include "TextComponent.h"

dae::MenuHUDComponent::MenuHUDComponent(GameObject* ownerPtr, TextComponent* markerTextComponent, std::vector<TextComponent*> buttons):
	ComponentBase(ownerPtr)
	, m_MarkerTextComponentPtr{markerTextComponent}
	, m_Buttons{std::move(buttons)}
{
}

void dae::MenuHUDComponent::Start()
{
	auto gameHandlerObject = SceneManager::GetInstance().GetDontDestroyOnLoadScene()->GetGameObjectsWithTag(make_sdbm_hash("GameHandler"));

	m_GameHandlerComponentPtr = gameHandlerObject.front()->GetComponent<GameHandlerComponent>();
}

void dae::MenuHUDComponent::Update(float)
{
}

void dae::MenuHUDComponent::LateUpdate(float)
{
}

void dae::MenuHUDComponent::Render() const
{
}

void dae::MenuHUDComponent::ChangeMarkerIndexBy(int delta)
{
	m_CurrentIndex += delta;

	m_CurrentIndex = std::clamp(m_CurrentIndex, 0, static_cast<int>(m_IndexToMode.size() - 1));

	UpdateMarkerPosition();
}

void dae::MenuHUDComponent::UpdateMarkerPosition() const
{
	auto markerPos = m_MarkerTextComponentPtr->GetLocalTransform().GetPosition();

	auto newMarkerPosY = m_Buttons[m_CurrentIndex]->GetLocalTransform().GetPosition().y;

	m_MarkerTextComponentPtr->SetLocalPosition(markerPos.x, newMarkerPosY);
}

void dae::MenuHUDComponent::ChooseMode() const
{
	m_GameHandlerComponentPtr->SetGameMode(m_IndexToMode.at(m_CurrentIndex));
	m_GameHandlerComponentPtr->SwitchLevel("Level1");
}





