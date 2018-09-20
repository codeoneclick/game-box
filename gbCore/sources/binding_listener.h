#pragma once

#include "main_headers.h"
#include "declarations.h"
#include "i_binding_listener.h"
#include "binding_property.h"

namespace gb
{
	template<typename T>
	class binding_listener : public i_binding_listener
	{
	private:

	public:

		virtual ~binding_listener() = default;
		virtual void on_changed(const i_binding_property_shared_ptr& prop) {
			const auto prop_instance = std::static_pointer_cast<binding_property<T>>(prop);
			on_changed(prop, prop_instance->old_value(), prop_instance->get());
		};

		virtual void on_changed(const i_binding_property_shared_ptr& prop, const T& ovalue, const T& nvalue) = 0;
	};;
};


