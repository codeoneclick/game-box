#pragma once

#include "main_headers.h"
#include "declarations.h"
#include "ns_declarations.h"
#include "i_binding_model.h"

namespace game
{
	class binding_board_record : public gb::i_binding_record
	{
	private:

	protected:

	public:

		STTI_CLASS_GUID(binding_board_record, gb::i_binding_record::g_guids_container, override);

		PROP(public, binding_board_record, cols, ui32, 0);
		PROP(public, binding_board_record, rows, ui32, 0);
	};

	class binding_board_model : public gb::binding_model<binding_board_record>
	{
	private:

	protected:

	public:

		STTI_CLASS_GUID(binding_board_model, gb::i_binding_model::g_guids_container, override);

		PROP_DECL_R(cols);
		PROP_DECL_W(cols);
		PROP_DECL_R(rows);
		PROP_DECL_W(rows);
	};
};
