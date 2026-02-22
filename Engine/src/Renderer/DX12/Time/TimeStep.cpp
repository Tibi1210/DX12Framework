#include "pch.h"
#include "TimeStep.h"


namespace Engine {

	namespace Timer {
		TimeStep::TimeStep()
		{
			startTime = std::chrono::steady_clock::now();
			deltaTime = startTime;
		}

		float TimeStep::Tick()
		{
			const std::chrono::steady_clock::time_point lastTime = deltaTime;
			deltaTime = std::chrono::steady_clock::now();

			const std::chrono::duration<float> frameTime = deltaTime - lastTime;

			return frameTime.count();

		}

		float TimeStep::GetTotalTime()
		{
			const std::chrono::duration<float> totalTime = std::chrono::steady_clock::now() - startTime;
			return totalTime.count();
		}
	}
}