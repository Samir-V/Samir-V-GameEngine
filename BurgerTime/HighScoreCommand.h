#pragma once
#include "Command.h"
#include "GameObject.h"
#include "HighScoreInputComponent.h"
#include "GameHandlerComponent.h"

namespace svengine
{
	class HighScoreCommand : public Command
	{
	public:

		HighScoreCommand(const GameObject* highScoreInputObject)
		{
			m_HighScoreInputComponentPtr = highScoreInputObject->GetComponent<HighScoreInputComponent>();
		}
		virtual ~HighScoreCommand() override = default;

	protected:

		HighScoreInputComponent* GetHighScoreInputComponent() const { return m_HighScoreInputComponentPtr; }

	private:

		HighScoreInputComponent* m_HighScoreInputComponentPtr;
	};

	class ConfirmLetter : public HighScoreCommand
	{
	public:

		ConfirmLetter(const GameObject* highScoreInputObject) : HighScoreCommand(highScoreInputObject)
		{
		}

		void Execute() override
		{
			GetHighScoreInputComponent()->ConfirmLetter();
		}
	};

	class RemoveLetter : public HighScoreCommand
	{
	public:

		RemoveLetter(const GameObject* highScoreInputObject) : HighScoreCommand(highScoreInputObject)
		{
		}

		void Execute() override
		{
			GetHighScoreInputComponent()->RemoveLetter();
		}
	};

	class ConfirmName : public HighScoreCommand
	{
	public:

		ConfirmName(const GameObject* highScoreInputObject) : HighScoreCommand(highScoreInputObject)
		{
			auto gameHandlerObj = SceneManager::GetInstance().GetDontDestroyOnLoadScene()->GetGameObjectsWithTag(make_sdbm_hash("GameHandler")).front();
			m_GameHandlerComponentPtr = gameHandlerObj->GetComponent<GameHandlerComponent>();
		}

		void Execute() override
		{
			GetHighScoreInputComponent()->ConfirmName();
			m_GameHandlerComponentPtr->SwitchToScoreView();
		}

	private:

		GameHandlerComponent* m_GameHandlerComponentPtr;
	};

	class AlterLetterIndex : public HighScoreCommand
	{
	public:
		AlterLetterIndex(const GameObject* highScoreInputObject, int deltaIndex) : HighScoreCommand(highScoreInputObject), m_IndexDelta{ deltaIndex }
		{
			m_IndexDelta = std::clamp(m_IndexDelta, -1, 1);
		}

		void Execute() override
		{
			GetHighScoreInputComponent()->AlterLetterIndex(m_IndexDelta);
		}

	private:

		int m_IndexDelta;
	};
}