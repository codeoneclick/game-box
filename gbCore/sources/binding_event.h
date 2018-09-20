#pragma once

#include "main_headers.h"
#include "declarations.h"

namespace gb
{
	template <unsigned int NUM>
	struct binding_event_params
	{
		template <typename... F_ARGS, typename... T_ARGS, typename... ARGS>
		static void forward(std::function<void(F_ARGS ... fargs)> function, std::tuple<T_ARGS...> const& targs, ARGS ... args)
		{
			binding_event_params<NUM - 1>::forward(function, targs, std::get<NUM - 1>(targs), args...);
		}
	};

	template <>
	struct binding_event_params<0>
	{
		template <typename... F_ARGS, typename... T_ARGS, typename... ARGS>
		static void forward(std::function<void(F_ARGS ... fargs)> function, std::tuple<T_ARGS...> const&, ARGS ... args)
		{
			function(args...);
		}
	};

	template <typename... F_ARGS, typename... T_ARGS>
	void forward(std::function<void(F_ARGS ... fargs)> function, std::tuple<T_ARGS...> const& targs)
	{
		binding_event_params<sizeof...(T_ARGS)>::forward(function, targs);
	};

	class i_binding_event_callback : public std::enable_shared_from_this<i_binding_event_callback>
	{
	public:

		virtual ~i_binding_event_callback() = default;
	};

	template <class FUCTION, class... ARGS>
	class binding_task_callback : public i_binding_event_callback
	{
	protected:

		FUCTION m_function;
		std::tuple<ARGS...> m_args;

	public:

		explicit binding_task_callback(FUCTION function, ARGS ... args)
		{
			m_function = function;
			m_args = std::make_tuple(args...);
		};

		virtual ~binding_task_callback() = default;

		void call()
		{
			forward(m_function, m_args);
		};
	};

	class i_binding_event : public std::enable_shared_from_this<i_binding_event>
	{
	protected:

		std::string m_id;
		std::shared_ptr<i_binding_event_callback> m_callback;

	public:

		i_binding_event(const std::string& id, const std::shared_ptr<i_binding_event_callback>& callback)
		{
			m_id = id;
			m_callback = callback;
		}

		virtual ~i_binding_event() = default;

		std::string get_id() const
		{
			return m_id;
		}
	};

	class binding_event_model_prop_callback : public i_binding_event_callback
	{
	public:

		typedef std::function<void(const i_binding_model_shared_ptr&, const i_binding_property_shared_ptr&)> callback_t;

	protected:

		callback_t m_callback;

	public:

		explicit binding_event_model_prop_callback(const callback_t& callback)
		{
			m_callback = callback;
		};

		virtual ~binding_event_model_prop_callback() = default;

		void call(const i_binding_model_shared_ptr& model, const i_binding_property_shared_ptr& prop)
		{
			m_callback(model, prop);
		}
	};

	class binding_event_model_prop : public i_binding_event
	{
	public:

		binding_event_model_prop(ctti_guid_t record_id, const std::string& prop_id, const std::shared_ptr<i_binding_event_callback>& callback) : i_binding_event(generate_guid(record_id, prop_id), callback)
		{
		}

		virtual ~binding_event_model_prop() = default;

		void call(const i_binding_model_shared_ptr& model, const i_binding_property_shared_ptr& prop)
		{
			std::static_pointer_cast<binding_event_model_prop_callback>(m_callback)->call(model, prop);
		}

		static std::string generate_guid(ctti_guid_t record_id, const std::string& prop_id)
		{
			std::stringstream ss;
			ss << static_cast<i32>(record_id) << prop_id;
			return ss.str();
		}
	};

	template <typename... ARGS>
	class binding_event_callback : public i_binding_event_callback
	{
	public:

		typedef std::function<void(ARGS ...)> callback_t;

	protected:

		callback_t m_callback;

	public:

		explicit binding_event_callback(const callback_t& callback)
		{
			m_callback = callback;
		};

		virtual ~binding_event_callback() = default;

		void call(ARGS ... args)
		{
			m_callback(args...);
		}
	};

	template <typename... ARGS>
	class bindning_event : public i_binding_event
	{
	public:

		bindning_event(const std::string& id, const std::shared_ptr<i_binding_event_callback>& callback) :i_binding_event(id, callback)
		{
		}

		void call(ARGS ... args)
		{
			std::static_pointer_cast<binding_event_callback<ARGS...>>(m_callback)->call(args...);
		}
	};
};
