#pragma once

#include "main_headers.h"
#include "declarations.h"

namespace gb
{
	class i_binding
	{
	public:

		virtual void subscribe(const i_binding_listener_shared_ptr& listener) = 0;
		virtual void unsubscribe(const i_binding_listener_shared_ptr& listener) = 0;
		virtual void unsubscribe_all() = 0;
	};
};

