#pragma once

#include "main_headers.h"
#include "declarations.h"
#include "i_binding_coding.h"
#include "i_binding_dispatcher.h"

namespace gb
{
	class i_binding_property : public i_binding_dispatcher, public std::enable_shared_from_this<i_binding_property>
	{
	protected:

		std::string m_id = "undefined";

	public:

		i_binding_property() = default;
		virtual ~i_binding_property() = default;

		std::string get_id() const 
		{
			return m_id;
		};

		void set_id(const std::string& id)
		{
			m_id = id;
		};
	};
};