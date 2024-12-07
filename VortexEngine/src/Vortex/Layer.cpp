#include "Vxpch.h"
#include "Layer.h"

namespace Vortex {
	Layer::Layer(const std::string& debugName) : m_debugName(debugName) {

	}

	Layer::~Layer() {

	}

	void Layer::OnAttach() {}
	void Layer::OnDetach() {}
	void Layer::OnUpdate() {}
	void Layer::OnEvent(Event& event) {}
}
  