#pragma once

#include "Command.h"
#include "GameObject.h"
#include "MoveComponent.h"
#include "PeterPepperComponent.h"
#include "ServiceLocator.h"

class PeterPepperCommand : public Command
{
public:

	PeterPepperCommand(dae::GameObject* peterPepper) : m_PeterPepperObjectPtr(peterPepper)
	{
		m_PeterPepperComponentPtr = peterPepper->GetComponent<dae::PeterPepperComponent>();
		m_MoveComponentPtr = peterPepper->GetComponent<dae::MoveComponent>();
	}
	virtual ~PeterPepperCommand() override = default;

protected:

	dae::GameObject* GetPeterPepperObject() const { return m_PeterPepperObjectPtr; }
	dae::PeterPepperComponent* GetPeterPepperComponent() const { return m_PeterPepperComponentPtr; }
	dae::MoveComponent* GetMoveComponent() const { return m_MoveComponentPtr; }

private:

	dae::GameObject* m_PeterPepperObjectPtr;
	dae::PeterPepperComponent* m_PeterPepperComponentPtr;
	dae::MoveComponent* m_MoveComponentPtr;
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
		GetMoveComponent()->SetDirection(m_Direction);
	}

private:

	glm::vec2 m_Direction;
};

class SprayPepper : public PeterPepperCommand
{
public:

	SprayPepper(dae::GameObject* peterPepper) : PeterPepperCommand(peterPepper) {};

	void Execute() override
	{
		GetPeterPepperComponent()->ChangeState(std::make_unique<dae::SprayingState>());
	}
};





