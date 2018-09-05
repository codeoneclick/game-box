#pragma once

#include "main_headers.h"
#include "declarations.h"
#include "binding_slot.h"

namespace gb
{
	template <typename T>
	class binding_multi_slot : public i_binding_slot
	{
	private:

		std::vector<std::pair<std::shared_ptr<binding_property<T>>, std::shared_ptr<binding_connection<T>>>> m_connections;

	public:

		binding_multi_slot() = default;
		~binding_multi_slot()
		{
			detach_all();
		};

		binding_multi_slot(const binding_multi_slot& other) = delete;
		binding_multi_slot(binding_multi_slot&& other) noexcept = delete;
		binding_multi_slot& operator=(const binding_multi_slot& other) = delete;
		binding_multi_slot& operator=(binding_multi_slot&& other) noexcept = delete;

		std::shared_ptr<binding_connection<T>> attach(const std::shared_ptr<binding_property<T>> prop, const std::function<void(const T&, const T&)>& callback) {
			std::shared_ptr<binding_connection<T>> connection = nullptr;
			if (callback)
			{
				connection = std::make_shared<binding_connection<T>>(callback);
				m_connections.push_back(std::make_pair(prop, connection));
				prop->subscribe(connection);
			}
			return connection;
		};

		void detach(const std::shared_ptr<binding_connection<T>>& connection) 
		{
			m_connections.erase(std::remove_if(m_connections.begin(), m_connections.end(), [connection](const std::pair<std::shared_ptr<binding_property<T>>, std::shared_ptr<binding_connection<T>>>& it) {
				if (it.second == connection) 
				{
					it.first->unsubscribe(connection);
				}
				return it.second == connection;
			}), m_connections.end());
		};

		void detach_all() override 
		{
			std::for_each(m_connections.begin(), m_connections.end(), [](const std::pair<std::shared_ptr<binding_property<T>>, std::shared_ptr<binding_connection<T>>>& it) {
				it.first->unsubscribe(it.second);
			});
			m_connections.clear();
		};
	};
};

