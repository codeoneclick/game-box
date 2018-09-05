#include "binding_element_model.h"

namespace game
{
	void binding_element_model::add_submodel(const std::shared_ptr<gb::i_binding_model>& submodel)
	{
		gb::binding_model<binding_element_record>::add_submodel(submodel);
		if (submodel->is_base_of(binding_sub_element_model<binding_sub_element_record>::class_guid()))
		{
			m_sub_elements_stack.push_back(std::static_pointer_cast<binding_sub_element_model<binding_sub_element_record>>(submodel));
		}
	}

	void binding_element_model::remove_submodel(const std::shared_ptr<gb::i_binding_model>& submodel)
	{
		gb::binding_model<binding_element_record>::remove_submodel(submodel);
		const auto submodel_from_stack = m_sub_elements_stack.back();
		if (submodel_from_stack->is_base_of(binding_sub_element_model<binding_sub_element_record>::class_guid()))
		{
			if (submodel_from_stack == submodel)
			{
				m_sub_elements_stack.pop_back();
			}
			else
			{
				assert(false && "can't remove current submodel from top of stack");
			}
		}
	}

	std::shared_ptr<binding_sub_element_model<binding_sub_element_record>> binding_element_model::get_current_sub_element() const
	{
		auto sub_element = !m_sub_elements_stack.empty() ? m_sub_elements_stack.back() : nullptr;

		return sub_element;
	}
}