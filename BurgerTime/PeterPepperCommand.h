#pragma once

#include "Command.h"
#include "GameObject.h"
#include "MoveComponent.h"
#include "PeterPepperComponent.h"

class PeterPepperCommand : public Command
{
public:

	PeterPepperCommand(svengine::GameObject* peterPepper) : m_PeterPepperObjectPtr{peterPepper}
	{
		m_PeterPepperComponentPtr = peterPepper->GetComponent<svengine::PeterPepperComponent>();
		m_MoveComponentPtr = peterPepper->GetComponent<svengine::MoveComponent>();
	}
	virtual ~PeterPepperCommand() override = default;

protected:

	svengine::GameObject* GetPeterPepperObject() const { return m_PeterPepperObjectPtr; }
	svengine::PeterPepperComponent* GetPeterPepperComponent() const { return m_PeterPepperComponentPtr; }
	svengine::MoveComponent* GetMoveComponent() const { return m_MoveComponentPtr; }

private:

	svengine::GameObject* m_PeterPepperObjectPtr;
	svengine::PeterPepperComponent* m_PeterPepperComponentPtr;
	svengine::MoveComponent* m_MoveComponentPtr;
};

class MoveInDirection : public PeterPepperCommand
{
public:

	MoveInDirection(svengine::GameObject* peterPepper, const glm::vec2& direction) : PeterPepperCommand(peterPepper), m_Direction{direction}
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

	SprayPepper(svengine::GameObject* peterPepper) : PeterPepperCommand(peterPepper) {};

	void Execute() override
	{
		GetPeterPepperComponent()->SprayPepper();
	}
};





