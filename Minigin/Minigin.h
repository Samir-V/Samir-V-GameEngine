#pragma once
#include <functional>

namespace svengine
{
	class Minigin
	{
	public:
		explicit Minigin();
		~Minigin();
		void Run(const std::function<void()>& load);

		Minigin(const Minigin& other) = delete;
		Minigin(Minigin&& other) = delete;
		Minigin& operator=(const Minigin& other) = delete;
		Minigin& operator=(Minigin&& other) = delete;

		static constexpr float FIXED_TIME_STEP = 0.01666666666666666666666666666667f;
	};
}