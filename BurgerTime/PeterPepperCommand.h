#pragma once

#include "Command.h"
#include "GameObject.h"
#include "MoveComponent.h"
#include "PeterPepperComponent.h"

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

class DamageCharacter : public PeterPepperCommand
{
public:
	DamageCharacter(dae::GameObject* peterPepper, int damageAmount) : PeterPepperCommand(peterPepper), m_DamageAmount{ damageAmount } {};

	void Execute() override
	{
		GetPeterPepperComponent()->Damage(m_DamageAmount);
	}

private:

	int m_DamageAmount;
};

class KillEnemy : public PeterPepperCommand
{
public:
	KillEnemy(dae::GameObject* peterPepper) : PeterPepperCommand(peterPepper) {};

	void Execute() override
	{
		GetPeterPepperComponent()->GetEnemyKilledEvent()->NotifyObservers(Event(make_sdbm_hash("EnemyKilled")), GetPeterPepperObject());
	}
};

class KillSmallEnemy : public PeterPepperCommand
{
public:
	KillSmallEnemy(dae::GameObject* peterPepper) : PeterPepperCommand(peterPepper) {};

	void Execute() override
	{
		GetPeterPepperComponent()->GetEnemyKilledEvent()->NotifyObservers(Event(make_sdbm_hash("SmallEnemyKilled")), GetPeterPepperObject());
	}
};





