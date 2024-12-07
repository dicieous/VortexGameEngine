#include "Vxpch.h"
#include "LayerStack.h"

namespace Vortex {

	LayerStack::LayerStack() {
		m_LayerInsert = m_Layers.begin();
	}

	LayerStack::~LayerStack() {
		for (Layer* layer : m_Layers) {
			delete layer;
		}
	}

	void LayerStack::PushLayer(Layer* layer) {
		m_LayerInsert = m_Layers.emplace(m_LayerInsert, layer);
		m_LayerInsert++;
	}

	void LayerStack::PushOverlay(Layer* overlay) {
		m_Layers.emplace_back(overlay);
	}

	void LayerStack::PopLayer(Layer* layer) {

		auto itr = std::find(m_Layers.begin(), m_Layers.end(), layer);

		if (itr != m_Layers.end()) {
			m_Layers.erase(itr);
			m_LayerInsert--;
		}
	}

	void LayerStack::PopOverlay(Layer* overlay) {

		auto itr = std::find(m_Layers.begin(), m_Layers.end(), overlay);

		if (itr != m_Layers.end())
		{
			m_Layers.erase(itr);
		}
	}
}
