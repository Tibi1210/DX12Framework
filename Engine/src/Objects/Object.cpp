#include "pch.h"
#include "Object.h"

namespace Engine {

	void Object::Release() {
		for (auto& obj : elements) {
			obj.get()->Release();
		}
	}
}