#include "Controller.h"

#include "Windows.h"
#include "Xinput.h"

class Controller::ControllerInputImpl
{
public:

	ControllerInputImpl(int index) : m_ControllerIndex{ index }{}

	void ProcessControllerInput();
	bool IsPressedThisFrame(unsigned int key) const;
	bool IsReleasedThisFrame(unsigned int key) const;
	bool IsHeld(unsigned int key) const;

private:
	XINPUT_STATE m_PreviousState{};
	XINPUT_STATE m_CurrentState{};

	WORD m_KeysPressedThisFrame{};
	WORD m_KeysReleasedThisFrame{};

	int m_ControllerIndex;
};

void Controller::ControllerInputImpl::ProcessControllerInput()
{
	CopyMemory(&m_PreviousState, &m_CurrentState, sizeof(XINPUT_STATE));
	ZeroMemory(&m_CurrentState, sizeof(XINPUT_STATE));
	XInputGetState(m_ControllerIndex, &m_CurrentState);

	auto buttonChanges = m_CurrentState.Gamepad.wButtons ^ m_PreviousState.Gamepad.wButtons;
	m_KeysPressedThisFrame = buttonChanges & m_CurrentState.Gamepad.wButtons;
	m_KeysReleasedThisFrame = buttonChanges & (~m_CurrentState.Gamepad.wButtons);
}

bool Controller::ControllerInputImpl::IsPressedThisFrame(unsigned key) const
{
	return m_KeysPressedThisFrame & key;
}

bool Controller::ControllerInputImpl::IsReleasedThisFrame(unsigned key) const
{
	return m_KeysReleasedThisFrame & key;
}

bool Controller::ControllerInputImpl::IsHeld(unsigned key) const
{
	return m_CurrentState.Gamepad.wButtons & key;
}




// Main controller implementation

Controller::Controller(int index) :
	m_pImpl{ std::make_unique<ControllerInputImpl>(index) },
	m_ControllerIndex{ index }
{
}

Controller::~Controller() = default;



void Controller::ProcessInput() const
{
	m_pImpl->ProcessControllerInput();
}

bool Controller::IsPressedThisFrame(unsigned int key) const
{
	return m_pImpl->IsPressedThisFrame(key);
}

bool Controller::IsReleasedThisFrame(unsigned int key) const
{
	return m_pImpl->IsReleasedThisFrame(key);
}

bool Controller::IsHeld(unsigned int key) const
{
	return m_pImpl->IsHeld(key);
}

int Controller::GetControllerIndex() const
{
	return m_ControllerIndex;
}





