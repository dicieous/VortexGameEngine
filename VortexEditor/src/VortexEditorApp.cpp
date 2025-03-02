#include<Vortex.h>
#include <Vortex/Core/EntryPoint.h>

#include "EditorLayer.h"

namespace Vortex {

	class VortexEditor : public Application {
	public:
		VortexEditor(const Vortex::ApplicationSpecifications& Specs)
			:Application(Specs)
		{
			PushLayer(new EditorLayer());
		}

		~VortexEditor() {

		}
	};

	Vortex::Application* Vortex::CreateApplication() {
		ApplicationSpecifications Specs;
		Specs.Name = "Vortex Engine";
		return new VortexEditor(Specs);
	}

}