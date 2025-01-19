#include<Vortex.h>
#include <Vortex/Core/EntryPoint.h>

#include "EditorLayer.h"

namespace Vortex {

	class VortexEditor : public Application {
	public:
		VortexEditor()
			:Application("Vortex Editor")
		{
			PushLayer(new EditorLayer());
		}

		~VortexEditor() {

		}
	};

	Vortex::Application* Vortex::CreateApplication() {

		return new VortexEditor();
	}

}