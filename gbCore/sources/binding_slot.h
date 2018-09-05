#pragma once

#include "main_headers.h"
#include "declarations.h"
#include "binding_property.h"
#include "binding_connection.h"

namespace gb
{
	class i_binding_slot : public std::enable_shared_from_this<i_binding_slot>
	{
	public:

		i_binding_slot() = default;
		virtual ~i_binding_slot() = default;

		i_binding_slot(const i_binding_slot& other) = delete;
		i_binding_slot(i_binding_slot&& other) noexcept = delete;
		i_binding_slot& operator=(const i_binding_slot& other) = delete;
		i_binding_slot& operator=(i_binding_slot&& other) noexcept = delete;

		virtual void detach_all() = 0;
	};

	template <typename T>
	class binding_slot : public i_binding_slot
	{
	private:

		std::shared_ptr<binding_property<T>> m_prop;
		std::vector<std::shared_ptr<binding_connection<T>>> m_connections;

	public:

		explicit binding_slot(const std::shared_ptr<binding_property<T>>& prop)
		{
			m_prop = prop;
		}

		~binding_slot()
		{
			detach_all();
		}

		binding_slot(const binding_slot& other) = delete;
		binding_slot(binding_slot&& other) noexcept = delete;
		binding_slot& operator=(const binding_slot& other) = delete;
		binding_slot& operator=(binding_slot&& other) noexcept = delete;

		std::shared_ptr<binding_connection<T>> attach(const std::function<void(const T&, const T&)>& callback)
		{
			std::shared_ptr<binding_connection<T>> connection = nullptr;
			if (callback)
			{
				connection = std::make_shared<binding_connection<T>>(callback);
				m_connections.push_back(connection);
				m_prop->subscribe(connection);
			}
			return connection;
		}

		void detach(const std::shared_ptr<binding_connection<T>>& connection)
		{
			std::remove_if(m_connections.begin(), m_connections.end(), [connection](const std::shared_ptr<binding_connection<T>>& it) {
				return it == connection;
			});
			m_prop->unsubscribe(connection);
		}

		void detach_all() override
		{
			for (const auto& connection : m_connections)
			{
				m_prop->unsubscribe(connection);
			}
			m_connections.clear();
		}
	};
};