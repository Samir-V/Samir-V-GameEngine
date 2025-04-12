#pragma once

#include "Command.h"
#include "GameObject.h"
#include "PeterPepperComponent.h"
#include "ServiceLocator.h"

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

class DamageCharacter : public PeterPepperCommand
{
public:
	DamageCharacter(dae::GameObject* peterPepper, int damageAmount) : PeterPepperCommand(peterPepper), m_DamageAmount{ damageAmount } {};

	void Execute() override
	{
		GetPeterPepperComponent()->Damage(m_DamageAmount);
		ServiceLocator::GetSoundSystem().Play("BigFileCheck.mp3", 0.1f);
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





