#include "pch.h"
#include "InputHandler.h"


namespace Engine {

	void InputHandler::handleInput(const int event, const int x, const int y) {
		switch (event) {
			case WM_MOUSEMOVE: {
				mouseX = x;
				mouseY = y;
				break;
			}
			case WM_LBUTTONDOWN: {
				leftDown = true;
				break;
			}
			case WM_LBUTTONUP: {
				leftDown = false;
				break;
			}
			case WM_RBUTTONDOWN: {
				rightDown = true;
				break;
			}
			case WM_RBUTTONUP: {
				rightDown = false;
				break;
			}
			case WM_MBUTTONDOWN: {
				middleDown = true;
				break;
			}
			case WM_MBUTTONUP: {
				middleDown = false;
				break;
			}
			case WM_KEYDOWN: {
				keyValue = x;
				break;
			}
			case WM_KEYUP: {
				keyValue = NULL;
				break;
			}
			default: {

				break;
			}
		}
	}
}