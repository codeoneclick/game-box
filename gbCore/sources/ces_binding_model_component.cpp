#include "ces_binding_model_component.h"

namespace gb
{
	ces_binding_model_component::ces_binding_model_component()
	{

	}

	ces_binding_model_component::~ces_binding_model_component()
	{

	}

	void ces_binding_model_component::set_model(const i_binding_model_shared_ptr& model)
	{
		m_model = model;
	}
}