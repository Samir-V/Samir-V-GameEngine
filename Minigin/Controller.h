#pragma once

#include "Windows.h"
#include "Xinput.h"

class Controller final
{
public:

	Controller() = default;
	~Controller() = default;

	void ProcessInput();
	bool IsPressedThisFrame(unsigned int key) const;
	bool IsReleasedThisFrame(unsigned int key) const;
	bool IsHeld(unsigned int key) const;

private:

	XINPUT_STATE m_PreviousState{};
	XINPUT_STATE m_CurrentState{};

	WORD m_KeysPressedThisFrame{};
	WORD m_KeysReleasedThisFrame{};

	int m_ControllerIndex{ 0 };
};

