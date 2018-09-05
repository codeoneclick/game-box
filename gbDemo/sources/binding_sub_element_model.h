#pragma once

#include "main_headers.h"
#include "declarations.h"
#include "ns_declarations.h"
#include "i_binding_model.h"

namespace game
{
	enum class e_element_ids
	{
		UNDEFINED = 0,
		RED,
		GREEN,
		BLUE,
		YELLOW
	};

	class binding_sub_element_record : public gb::i_binding_record
	{
	private:

	protected:

	public:

		CTTI_CLASS_GUID(binding_sub_element_record, gb::i_binding_record::g_guids_container, override);

		PROP(public, binding_board_record, element_id, e_element_ids, e_element_ids::UNDEFINED);
	};

	template<typename T, typename = typename std::enable_if<std::is_base_of<binding_sub_element_record, T>::value>::type>
	class binding_sub_element_model : public gb::binding_model<T>
	{
	private:

	protected:

	public:

		CTTI_CLASS_GUID(binding_sub_element_model, gb::i_binding_model::g_guids_container, override);

		PROP_DECL_R(element_id);
	};

	template<typename TModel>
	using is_binding_sub_element_model_t = typename std::enable_if<std::is_base_of<binding_sub_element_model<typename TModel::record_t>, TModel>::value && std::is_base_of<binding_sub_element_record, typename TModel::record_t>::value>::type;
};