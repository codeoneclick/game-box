#include "binding_global_dispatcher.h"
#include "binding_event.h"

namespace gb
{
	std::shared_ptr<global_dispatcher> global_dispatcher::m_instance = nullptr;

	void global_dispatcher::subscribe(const i_binding_event_shared_ptr& event)
	{
		const auto it = m_listeners.find(event->get_id());
		if (it == m_listeners.end())
		{
			m_listeners.insert(std::make_pair(event->get_id(), std::vector<i_binding_event_shared_ptr>()));
		}
		m_listeners[event->get_id()].push_back(event);
	};

	void global_dispatcher::unsubscribe(const i_binding_event_shared_ptr& event)
	{
		const auto it1 = m_listeners.find(event->get_id());
		if (it1 != m_listeners.end())
		{
			const auto it2 = std::find(m_listeners[event->get_id()].begin(), m_listeners[event->get_id()].end(), event);
			if (it2 != m_listeners[event->get_id()].end())
			{
				m_listeners[event->get_id()].erase(it2);
			}
		}
	};
}