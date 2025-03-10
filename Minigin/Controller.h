#pragma once
#include <memory>

class Controller final
{
public:

	Controller(int index);
	~Controller();

	void ProcessInput() const;
	bool IsPressedThisFrame(unsigned int key) const;
	bool IsReleasedThisFrame(unsigned int key) const;
	bool IsHeld(unsigned int key) const;

	int GetControllerIndex() const;

private:

	class ControllerInputImpl;

	std::unique_ptr<ControllerInputImpl> m_pImpl;
};

