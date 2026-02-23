#pragma once

namespace Engine {

	class RENDER_API MouseInputHandler {

	public:
		MouseInputHandler() = default;
		void handleMouseInput(const int event, const int x, const int y);

		int mouseX = 0;
		int mouseY = 0;

		bool leftDown = false;
		bool rightDown = false;
		bool middleDown = false;

	private:
	};

}

