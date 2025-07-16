#include "MenuHUDComponent.h"

#include <utility>

#include "Scene.h"
#include "SceneManager.h"
#include "TextComponent.h"

svengine::MenuHUDComponent::MenuHUDComponent(GameObject* ownerPtr, TextComponent* markerTextComponent, std::vector<TextComponent*> buttons):
	ComponentBase(ownerPtr)
	, m_MarkerTextComponentPtr{markerTextComponent}
	, m_Buttons{std::move(buttons)}
{
}

void svengine::MenuHUDComponent::Start()
{
	const auto gameHandlerObject = SceneManager::GetInstance().GetDontDestroyOnLoadScene()->GetGameObjectsWithTag(make_sdbm_hash("GameHandler"));

	m_GameHandlerComponentPtr = gameHandlerObject.front()->GetComponent<GameHandlerComponent>();
}

void svengine::MenuHUDComponent::Update(float)
{
}

void svengine::MenuHUDComponent::LateUpdate(float)
{
}

void svengine::MenuHUDComponent::Render() const
{
}

void svengine::MenuHUDComponent::ChangeMarkerIndexBy(int delta)
{
	m_CurrentIndex += delta;

	m_CurrentIndex = std::clamp(m_CurrentIndex, 0, static_cast<int>(m_IndexToMode.size() - 1));

	UpdateMarkerPosition();
}

void svengine::MenuHUDComponent::UpdateMarkerPosition() const
{
	const auto markerPos = m_MarkerTextComponentPtr->GetLocalTransform().GetPosition();

	const auto newMarkerPosY = m_Buttons[m_CurrentIndex]->GetLocalTransform().GetPosition().y;

	m_MarkerTextComponentPtr->SetLocalPosition(markerPos.x, newMarkerPosY);
}

void svengine::MenuHUDComponent::ChooseMode() const
{
	m_GameHandlerComponentPtr->StartMainGame(m_IndexToMode.at(m_CurrentIndex));
}





