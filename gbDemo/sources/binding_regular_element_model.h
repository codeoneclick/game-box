#pragma once

#include "main_headers.h"
#include "declarations.h"
#include "ns_declarations.h"
#include "binding_sub_element_model.h"

namespace game
{
	class binding_regular_element_record : public binding_sub_element_record
	{
	private:

	protected:

	public:

		STTI_CLASS_GUID(binding_regular_element_record, gb::i_binding_record::g_guids_container, override);
	};

	class binding_regular_element_model : public binding_sub_element_model<binding_regular_element_record>
	{
	private:

	protected:

	public:

		STTI_CLASS_GUID(binding_regular_element_model, gb::i_binding_model::g_guids_container, override);

		bool is_base_of(stti_guid_t base_guid) override;
	};
};
