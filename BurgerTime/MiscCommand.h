#pragma once
#include "Command.h"
#include "GameHandlerComponent.h"
#include "GameObject.h"
#include "ServiceLocator.h"

namespace dae
{
	class MiscCommand : public Command
	{
	public:

		MiscCommand(const GameObject* gameHandlerObject)
		{
			m_GameHandlerComponentPtr = gameHandlerObject->GetComponent<GameHandlerComponent>();
		}
		virtual ~MiscCommand() override = default;

	protected:

		GameHandlerComponent* GetGameHandlerComponent() const { return m_GameHandlerComponentPtr; }

	private:

		GameHandlerComponent* m_GameHandlerComponentPtr;
	};

	class MuteSound : public MiscCommand
	{
	public:

		MuteSound(const GameObject* gameHandlerObject) : MiscCommand(gameHandlerObject)
		{
		}

		void Execute() override
		{
			m_Mute = !m_Mute;
			ServiceLocator::GetSoundSystem().SetMute(m_Mute);
		}

	private:

		bool m_Mute{ false };
	};


	class SkipLevel : public MiscCommand
	{
	public:

		SkipLevel(const GameObject* gameHandlerObject) : MiscCommand(gameHandlerObject)
		{
		}

		void Execute() override
		{
			GetGameHandlerComponent()->SkipLevel();
		}
	};

	class VisitHighScoreView : public MiscCommand
	{
	public:
		VisitHighScoreView(const GameObject* gameHandlerObject) : MiscCommand(gameHandlerObject)
		{
		}

		void Execute() override
		{
			GetGameHandlerComponent()->SwitchToScoreView();
		}
	};

	class ReturnToMenu : public MiscCommand
	{
	public:
		ReturnToMenu(const GameObject* gameHandlerObject) : MiscCommand(gameHandlerObject)
		{
		}

		void Execute() override
		{
			GetGameHandlerComponent()->ReturnToMenu();
		}
	};
}