#include "pch.h"
#include "InputHandler.h"

namespace Engine {

	void InputHandler::handleInput(Keys key, bool pressed){
        auto* ptr = reinterpret_cast<uint32_t*>(&keyboardState);

        const size_t word = static_cast<unsigned char>(key) >> 5;
        const uint32_t mask = 1u << (static_cast<unsigned char>(key) & 0x1F);

        if (pressed)
            ptr[word] |= mask;
        else
            ptr[word] &= ~mask;
	}

}