#include "ServiceLocator.h"

std::unique_ptr<SoundSystem> ServiceLocator::m_SSInstance = nullptr;