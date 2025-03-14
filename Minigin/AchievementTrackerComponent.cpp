#include "AchievementTrackerComponent.h"

#include <isteamuserstats.h>

#include "GameObject.h"
#include "PeterPepperComponent.h"

dae::AchievementTrackerComponent::AchievementTrackerComponent(GameObject* ownerPtr): ComponentBase(ownerPtr)
{
}

void dae::AchievementTrackerComponent::Update(float )
{
	
}

void dae::AchievementTrackerComponent::LateUpdate(float )
{
	
}

void dae::AchievementTrackerComponent::Render() const
{
	
}

void dae::AchievementTrackerComponent::SetLocalPosition(float x, float y)
{
	m_LocalTransform.SetPosition(x, y, 0.0f);
}


void dae::AchievementTrackerComponent::Notify(EventType event, GameObject* observedGameObject)
{
	switch (event)
	{
	case EventType::ScoreChanged:

		if (observedGameObject->GetComponent<PeterPepperComponent>()->GetScore() >= 500)
		{
			SteamUserStats()->SetAchievement("ACH_WIN_ONE_GAME");
			SteamUserStats()->StoreStats();
		}

		break;
	}
}


