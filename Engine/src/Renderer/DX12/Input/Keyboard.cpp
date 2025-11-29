#include "pch.h"
#include "Keyboard.h"

namespace Engine {




	void Keyboard::OnKeyPressed(){


            Keys key = static_cast<Keys>(RAWinput->data.keyboard.VKey);

            if ((RAWinput->data.keyboard.Flags & RI_KEY_BREAK) == 0) {
                switch (key){
                    case Keys::W:
                
                        break;
                    case Keys::A:
                        strafe += 0.5;
                        break;
                    case Keys::S:
                
                        break;
                    case Keys::D:
                        strafe -= 0.5;
                        break;
                    default:
                        // Handle other keys or ignore
                        break;
                }

            }
	}

}