#pragma once

#include "main_headers.h"
#include "declarations.h"

namespace gb
{
	class i_binding_listener : public std::enable_shared_from_this<i_binding_listener>
	{
	protected:

	public:

		virtual ~i_binding_listener() = default;
		virtual void on_changed(const i_binding_property_shared_ptr& prop) = 0;
	};
};