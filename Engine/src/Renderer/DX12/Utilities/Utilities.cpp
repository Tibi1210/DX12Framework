#include "pch.h"
#include "Utilities.h"

namespace Engine {
	namespace Utils {
		UINT CalcConstBufferAlignment(const UINT allocation){

			return (allocation+255) & ~255;
		}
	}
}