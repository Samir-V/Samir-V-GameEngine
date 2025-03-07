#include "Controller.h"

void Controller::ProcessInput()
{
	CopyMemory(&m_PreviousState, &m_CurrentState, sizeof(XINPUT_STATE));
	ZeroMemory(&m_CurrentState, sizeof(XINPUT_STATE));
	XInputGetState(m_ControllerIndex, &m_CurrentState);

	auto buttonChanges = m_CurrentState.Gamepad.wButtons ^ m_PreviousState.Gamepad.wButtons;
	m_KeysPressedThisFrame = buttonChanges & m_CurrentState.Gamepad.wButtons;
	m_KeysReleasedThisFrame = buttonChanges & (~m_CurrentState.Gamepad.wButtons);
}

bool Controller::IsPressedThisFrame(unsigned int key) const
{
	return m_KeysPressedThisFrame & key;
}

bool Controller::IsReleasedThisFrame(unsigned int key) const
{
	return m_KeysReleasedThisFrame & key;
}

bool Controller::IsHeld(unsigned int key) const
{
	return m_CurrentState.Gamepad.wButtons & key;
}

int Controller::GetControllerIndex() const
{
	return m_ControllerIndex;
}





