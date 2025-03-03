#pragma once
#include "Command.h"
#include "GameObject.h"

class GameActorCommand : public Command
{
public:

	GameActorCommand(dae::GameObject* actor) : m_ptrActor(actor){}
	virtual ~GameActorCommand() override;

protected:

	dae::GameObject* GetGameActor() const { return m_ptrActor; }

private:

	dae::GameObject* m_ptrActor;
};

