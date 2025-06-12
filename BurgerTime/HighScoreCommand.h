#pragma once
#include "Command.h"
#include "GameObject.h"
#include "HighScoreInputComponent.h"

namespace dae
{
	class HighScoreCommand : public Command
	{
	public:

		HighScoreCommand(const dae::GameObject* highScoreInputObject)
		{
			m_HighScoreInputComponentPtr = highScoreInputObject->GetComponent<dae::HighScoreInputComponent>();
		}
		virtual ~HighScoreCommand() override = default;

	protected:

		dae::HighScoreInputComponent* GetHighScoreInputComponent() const { return m_HighScoreInputComponentPtr; }

	private:

		dae::HighScoreInputComponent* m_HighScoreInputComponentPtr;
	};

	class ConfirmLetter : public HighScoreCommand
	{
	public:

		ConfirmLetter(const dae::GameObject* highScoreInputObject) : HighScoreCommand(highScoreInputObject)
		{
		}

		void Execute() override
		{
			GetHighScoreInputComponent()->ConfirmLetter();
		}
	};

	class ConfirmName : public HighScoreCommand
	{
	public:

		ConfirmName(const dae::GameObject* highScoreInputObject) : HighScoreCommand(highScoreInputObject)
		{
		}

		void Execute() override
		{
			GetHighScoreInputComponent()->ConfirmName();
		}
	};

	class AlterLetterIndex : public HighScoreCommand
	{
	public:
		AlterLetterIndex(const dae::GameObject* highScoreInputObject, int deltaIndex) : HighScoreCommand(highScoreInputObject), m_IndexDelta{ deltaIndex }
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