#pragma once

namespace Engine {

	class RENDER_API InputHandler {

	public:
		InputHandler() = default;
		void handleInput(const int event, const int x, const int y);

		int mouseX = 0;
		int mouseY = 0;

		bool leftDown = false;
		bool rightDown = false;
		bool middleDown = false;

		int keyValue = NULL;

	private:
	};

}

