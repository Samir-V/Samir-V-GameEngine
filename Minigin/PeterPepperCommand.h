#pragma once

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

class MoveInDirection : public PeterPepperCommand
{
public:

	MoveInDirection(dae::GameObject* peterPepper, const glm::vec2& direction) : PeterPepperCommand(peterPepper), m_Direction{direction}
	{
		m_Direction.x = std::clamp(m_Direction.x, -1.0f, 1.0f);
		m_Direction.y = std::clamp(m_Direction.y, -1.0f, 1.0f);
	}

	void Execute() override
	{
		GetPeterPepperComponent()->AddInputToDirection(m_Direction);
	}

private:

	glm::vec2 m_Direction;
};





