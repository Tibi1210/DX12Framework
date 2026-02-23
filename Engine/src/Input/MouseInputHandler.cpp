#include "pch.h"
#include "MouseInputHandler.h"


namespace Engine {

	void MouseInputHandler::handleMouseInput(const int event, const int x, const int y) {
		switch (event) {
			case 0: {
				mouseX = x;
				mouseY = y;
				break;
			}
			case 1: {
				leftDown = true;
				break;
			}
			case -1: {
				leftDown = false;
				break;
			}
			case 2: {
				rightDown = true;
				break;
			}
			case -2: {
				rightDown = false;
				break;
			}
			case 3: {
				middleDown = true;
				break;
			}
			case -3: {
				middleDown = false;
				break;
			}
		}
	}

}