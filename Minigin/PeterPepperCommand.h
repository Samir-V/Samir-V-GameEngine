#pragma once
#include <iostream>

#include "Command.h"
#include "GameObject.h"
#include "PeterPepperComponent.h"

class PeterPepperCommand : public Command
{
public:

	PeterPepperCommand(dae::GameObject* peterPepper) : m_PeterPepperObjectPtr(peterPepper) { m_PeterPepperComponentPtr = peterPepper->GetComponent<dae::PeterPepperComponent>(); }
	virtual ~PeterPepperCommand() override = default;

protected:

	dae::GameObject* GetPeterPepperObject() const { return m_PeterPepperObjectPtr; }
	dae::PeterPepperComponent* GetPeterPepperComponent() const { return m_PeterPepperComponentPtr; }

private:

	dae::GameObject* m_PeterPepperObjectPtr;
	dae::PeterPepperComponent* m_PeterPepperComponentPtr;
};

class MoveUp : public PeterPepperCommand
{
public:

	MoveUp(dae::GameObject* peterPepper) : PeterPepperCommand(peterPepper) {}

	void Execute() override
	{
		GetPeterPepperComponent()->SetYDirection(-1.0f);
	}
};

class MoveDown : public PeterPepperCommand
{
public:

	MoveDown(dae::GameObject* peterPepper) : PeterPepperCommand(peterPepper) {}

	void Execute() override
	{
		GetPeterPepperComponent()->SetYDirection(1.0f);
	}
};

class MoveLeft : public PeterPepperCommand
{
public:

	MoveLeft(dae::GameObject* peterPepper) : PeterPepperCommand(peterPepper) {}

	void Execute() override
	{
		GetPeterPepperComponent()->SetXDirection(-1.0f);
	}
};

class MoveRight : public PeterPepperCommand
{
public:

	MoveRight(dae::GameObject* peterPepper) : PeterPepperCommand(peterPepper) {}

	void Execute() override
	{
		GetPeterPepperComponent()->SetXDirection(1.0f);
	}
};





