#pragma once

#include "main_headers.h"
#include "declarations.h"
#include "ns_declarations.h"
#include "i_binding_model.h"
#include "binding_sub_element_model.h"

namespace game
{
	enum class e_element_states
	{
		IDLE = 0,
		SPAWNING = 1 << 0,
		SWAPPING = 1 << 1,
		MOVING = 1 << 2,
		MATCHING = 1 << 3,
		REMOVING = 1 << 4
	};

	class binding_element_record : public gb::i_binding_record
	{
	private:

	protected:

	public:

		CTTI_CLASS_GUID(binding_element_record, gb::i_binding_record::g_guids_container, override);

		PROP(public, binding_element_record, state, e_element_states, e_element_states::IDLE);
	};

	class binding_element_model : public gb::binding_model<binding_element_record>
	{
	private:

		std::list<std::shared_ptr<binding_sub_element_model<binding_sub_element_record>>> m_sub_elements_stack;

	protected:

	public:

		CTTI_CLASS_GUID(binding_element_model, gb::i_binding_model::g_guids_container, override);

		void add_submodel(const std::shared_ptr<gb::i_binding_model>& submodel) override;
		void remove_submodel(const std::shared_ptr<gb::i_binding_model>& submodel) override;

		std::shared_ptr<binding_sub_element_model<binding_sub_element_record>> get_current_sub_element() const;

		template<typename T, typename = is_binding_sub_element_model_t<T>>
		std::shared_ptr<T> get_current_sub_element_as() const
		{
			const auto sub_element = get_current_sub_element();
			assert((sub_element->instance_guid() == T::class_guid()));

			return std::static_pointer_cast<T>(std::static_pointer_cast<gb::i_binding_model>(sub_element));
		};
	};
};