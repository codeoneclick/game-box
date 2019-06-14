#pragma once

#include "main_headers.h"
#include "declarations.h"
#include "ns_declarations.h"
#include "i_binding_model.h"

namespace game
{
	class binding_cell_record : public gb::i_binding_record
	{
	private:

	protected:

	public:

		STTI_CLASS_GUID(binding_cell_record, gb::i_binding_record::g_guids_container, override);

		PROP(public, binding_board_record, col, ui32, 0);
		PROP(public, binding_board_record, row, ui32, 0);
	};

	class binding_cell_model : public gb::binding_model<binding_cell_record>
	{
	private:

	protected:

	public:

		STTI_CLASS_GUID(binding_cell_model, gb::i_binding_model::g_guids_container, override);

		PROP_DECL_R(col);
		PROP_DECL_R(row);
	};
};
