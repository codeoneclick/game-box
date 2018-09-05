#pragma once

#include "main_headers.h"
#include "declarations.h"

namespace gb
{
	class global_dispatcher
	{
	private:

		static std::shared_ptr<global_dispatcher> m_instance;

		std::unordered_map<std::string, std::vector<i_binding_event_shared_ptr>> m_listeners;

	public:

		global_dispatcher() = default;
		~global_dispatcher() = default;

		static std::shared_ptr<global_dispatcher> get_instance()
		{
			if (m_instance == nullptr)
			{
				m_instance = std::make_shared<global_dispatcher>();
			}
			return m_instance;
		};

		template<typename T, typename... ARGS>
		void publish(const std::string& id, ARGS&&... args)
		{
			const auto it = m_listeners.find(id);
			if (it != m_listeners.end())
			{
				for (const auto& event : it->second)
				{
					std::static_pointer_cast<T>(event)->call(args...);
				}
			}
		};

		void subscribe(const i_binding_event_shared_ptr& event);
		void unsubscribe(const i_binding_event_shared_ptr& event);
	};
};
