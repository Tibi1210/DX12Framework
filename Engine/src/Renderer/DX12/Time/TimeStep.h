#pragma once
#include <chrono>

namespace Engine {

	namespace Timer {

		class RENDER_API TimeStep
		{
		public:

			TimeStep();

			float Tick();


		private:

			std::chrono::steady_clock::time_point time;
		};
	}

}

