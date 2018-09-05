#pragma once

#include "main_headers.h"
#include "declarations.h"
#include "i_binding.h"

namespace gb
{
	class i_binding_dispatcher : public i_binding
	{
	protected:

		std::vector<i_binding_listener_shared_ptr> m_listeners;

	public:

		virtual ~i_binding_dispatcher() = default;

		void subscribe(const i_binding_listener_shared_ptr& listener)
		{
			m_listeners.push_back(listener);
		}

		void unsubscribe(const i_binding_listener_shared_ptr& listener)
		{
			const auto it = std::find(m_listeners.begin(), m_listeners.end(), listener);
			if (it != m_listeners.end())
			{
				m_listeners.erase(it);
			}
			else
			{
				assert(false);
			}
		}

		void unsubscribe_all()
		{
			m_listeners.clear();
		}
	};
};

