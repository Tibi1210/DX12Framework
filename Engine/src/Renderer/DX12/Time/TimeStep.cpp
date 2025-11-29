#include "pch.h"
#include "TimeStep.h"


namespace Engine {

	namespace Timer {
		TimeStep::TimeStep()
		{
			time = std::chrono::steady_clock::now();
		}
		float TimeStep::Tick()
		{
			const std::chrono::steady_clock::time_point lastTime = time;
			time = std::chrono::steady_clock::now();

			const std::chrono::duration<float> frameTime = time - lastTime;

			return frameTime.count();

		}
	}
}