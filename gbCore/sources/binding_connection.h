#pragma once

#include "main_headers.h"
#include "declarations.h"
#include "binding_listener.h"

namespace gb
{
	template<typename T>
	class binding_connection : public binding_listener<T>
	{
	private:

		std::function<void(const T&, const T&)> m_callback;

		void call(const T& ovalue, const T& nvalue) 
		{
			return m_callback(ovalue, nvalue);
		};

	public:

		binding_connection(const std::function<void(const T&, const T&)>& callback) {
			m_callback = callback;
		};

		void on_changed(const i_binding_property_shared_ptr& prop, const T& ovalue, const T& nvalue) {
			call(ovalue, nvalue);
		};
	};
};

