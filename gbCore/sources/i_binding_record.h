#pragma once

#include "main_headers.h"
#include "declarations.h"
#include "binding_property.h"

namespace gb
{
	class i_binding_record : public std::enable_shared_from_this<i_binding_record>
	{
	protected:

		static std::set<ctti_guid_t> g_guids_container;

	public:

		CTTI_CLASS_GUID(i_binding_record, g_guids_container)

		PROP(public, record, is_assigned_to_view, bool, false)

		i_binding_record() = default;
		virtual ~i_binding_record() = default;
	};
};
