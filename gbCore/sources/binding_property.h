#pragma once

#include "main_headers.h"
#include "declarations.h"
#include "i_binding_property.h"
#include "i_binding_listener.h"

namespace gb
{
	template<typename T>
	class binding_property : public i_binding_coding<T>, public i_binding_property
	{
	private:

		T m_value;
		T m_old_value;

	protected:

		void init()
		{
			i_binding_coding<T>::getter = [=]()
			{
				return m_value;
			};

			i_binding_coding<T>::setter = [=](const T& value)
			{
				m_old_value = m_value;
				m_value = value;
				const auto listeners = m_listeners;
				if (!listeners.empty())
				{
					for (const auto& listener : listeners)
					{
						listener->on_changed(shared_from_this());
					}
				}
			};
		};

	public:

		using value_t = T;

		binding_property()
		{
			init();
		};

		binding_property(const T& value)
		{
			m_value = value;
			m_old_value = value;
			init();
		}

		virtual ~binding_property() = default;

		binding_property(const binding_property&) = delete;
		binding_property& operator= (const binding_property&) = delete;

		void set(const T& value) override
		{
			if (!binding_property<T>::setter)
			{
				assert(false);
			}
			binding_property<T>::setter(value);
		}

		T get() const override
		{
			if (!binding_property<T>::getter)
			{
				assert(false);
			}
			return binding_property<T>::getter();
		}

		T get_old() const
		{
			return m_old_value;
		}
	};
};

