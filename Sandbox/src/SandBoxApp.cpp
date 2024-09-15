#include <Vortex.h>

class SandBox : public Vortex::Application {
public:
	SandBox() {
	}

	~SandBox() {

	}
};

Vortex::Application* Vortex::CreateApplication() {

	return new SandBox();
}