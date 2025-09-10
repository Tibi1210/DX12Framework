
#include "Application.h"

int main() {

	using namespace Engine;

	Application engine;

	if (engine.Initialize()){
		while (engine.IsRunning()){
			engine.Run();
		}
	}
}