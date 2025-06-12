#pragma once
#include "Command.h"
#include "GameHandlerComponent.h"
#include "GameObject.h"
#include "ServiceLocator.h"

class MiscCommand : public Command
{
public:

	MiscCommand(const dae::GameObject* gameHandlerObject)
	{
		m_MenuGameHandlerComponentPtr = gameHandlerObject->GetComponent<dae::GameHandlerComponent>();
	}
	virtual ~MiscCommand() override = default;

protected:

	dae::GameHandlerComponent* GetGameHandlerComponent() const { return m_MenuGameHandlerComponentPtr; }

private:

	dae::GameHandlerComponent* m_MenuGameHandlerComponentPtr;
};

class MuteSound : public MiscCommand
{
public:

	MuteSound(const dae::GameObject* gameHandlerObject): MiscCommand(gameHandlerObject)
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

	SkipLevel(const dae::GameObject* gameHandlerObject) : MiscCommand(gameHandlerObject)
	{
	}

	void Execute() override
	{
		GetGameHandlerComponent()->SkipLevel();
	}
};