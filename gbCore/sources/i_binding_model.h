#pragma once

#include "main_headers.h"
#include "declarations.h"
#include "i_binding_record.h"
#include "i_binding_listener.h"
#include "i_binding_property.h"
#include "binding_event.h"
#include "binding_global_dispatcher.h"

namespace gb
{
	class i_binding_model_storage
	{
	public:

		i_binding_model_storage() = default;
		virtual ~i_binding_model_storage() = default;

		static std::shared_ptr<std::list<i_binding_model_shared_ptr>> get_storage(ctti_guid_t guid);
		static bool is_storage_registered(ctti_guid_t guid);
		static void register_storage(ctti_guid_t guid);
	};

	class i_binding_model_pool
	{
	public:

		i_binding_model_pool() = default;
		virtual ~i_binding_model_pool() = default;

		static std::shared_ptr<std::list<i_binding_model_shared_ptr>> get_pool(ctti_guid_t guid);
		static bool is_pool_registered(ctti_guid_t guid);
		static void register_pool(ctti_guid_t guid);
	};

	class i_binding_model : public i_binding_listener 
	{
	protected:

		static std::set<ctti_guid_t> g_guids_container;

		std::weak_ptr<i_binding_model> m_parent;
		std::unordered_map<ctti_guid_t, std::shared_ptr<i_binding_model>> m_submodels;

		public:

		i_binding_model();
		virtual ~i_binding_model() = default;

		CTTI_CLASS_GUID(i_binding_model, g_guids_container)

		virtual void init() = 0;
		virtual void serialize() {};
		virtual void deserialize(const void* data) {};
		virtual void reset() = 0;

		virtual bool is_base_of(ctti_guid_t base_guid)
		{
			return base_guid == i_binding_model::instance_guid();
		};

		template<typename TModel>
		bool instance_of()
		{
			return instance_guid() == TModel::class_guid();
		}

		template<typename TModel>
		std::shared_ptr<TModel> get_parent() const
		{
			std::shared_ptr<TModel> result = nullptr;
			if (!m_parent.expired())
			{
				result = std::static_pointer_cast<TModel>(m_parent.lock());
			}
			return result;
		};

		template<typename TModel>
		void set_parent(const std::shared_ptr<TModel>& parent)
		{
			static_assert(std::is_base_of<i_binding_model, TModel>::value, "TModel must derive from i_binding_model");
			m_parent = parent;
		};

		virtual void add_submodel(const std::shared_ptr<i_binding_model>& submodel)
		{
			const auto it = m_submodels.find(submodel->instance_guid());
			assert(it == m_submodels.end() && "can't add same model");

			if (it == m_submodels.end())
			{
				m_submodels[submodel->instance_guid()] = submodel;
				const auto instance = i_binding_model::shared_from_this();
				submodel->set_parent(std::static_pointer_cast<i_binding_model>(instance));
			}
		};

		virtual void remove_submodel(const std::shared_ptr<i_binding_model>& submodel)
		{
			submodel->detach_parent();

			const auto it = m_submodels.find(submodel->instance_guid());
			if (it != m_submodels.end())
			{
				m_submodels.erase(it);
			}
			else
			{
				assert(false && "can't remove nonexistent submodel");
			}
		};

		void remove_all_submodels()
		{
			std::for_each(m_submodels.begin(), m_submodels.end(), [=](const std::pair<ctti_guid_t, i_binding_model_shared_ptr>& it) {
				it.second->detach_parent();
			});
			m_submodels.clear();
		};

		template<typename TSubmodel>
		std::shared_ptr<TSubmodel> get_submodel() const
		{
			static_assert(std::is_base_of<i_binding_model, TSubmodel>::value, "TSubmodel must derive from i_binding_model");
			std::shared_ptr<TSubmodel> submodel = nullptr;
			auto class_guid = TSubmodel::class_guid();
			const auto it = m_submodels.find(class_guid);
			if (it != m_submodels.end())
			{
				submodel = std::static_pointer_cast<TSubmodel>(it->second);
			}
			return submodel;
		};

		std::unordered_map<ctti_guid_t, std::shared_ptr<i_binding_model>>& get_submodels()
		{
			return m_submodels;
		};

		void detach_parent()
		{
			m_parent.reset();
		};
	};

	template<typename T>
	class binding_model : public i_binding_model 
	{
	static_assert(std::is_base_of<i_binding_record, T>::value, "T must derive from i_binding_record");

	public:

		using record_t = T;

		CTTI_CLASS_GUID(binding_model<T>, i_binding_model::g_guids_container, override)

		PROP(public, binding_model<T>, record_data, std::shared_ptr<T>, nullptr)

		binding_model()
		{
			record_data->set(std::make_shared<T>());
			m_parent.reset();
		};

		virtual ~binding_model()
		{
			m_parent.reset();
			m_submodels.clear();
		};

		template<typename RESULT>
		static std::shared_ptr<RESULT> construct()
		{
			std::shared_ptr<RESULT> result = nullptr;

			if (!i_binding_model_pool::is_pool_registered(RESULT::class_guid()))
			{
				i_binding_model_pool::register_pool(RESULT::class_guid());
			}

			const auto pool = i_binding_model_pool::get_pool(RESULT::class_guid());
			if (!pool->empty())
			{
				result = std::static_pointer_cast<RESULT>(pool->back());
				pool->pop_back();
			}
			else
			{
				result = std::make_shared<RESULT>();
				result->init();
			}
			result->add_to_storage();
			return result;
		}

		static void destruct(const std::shared_ptr<binding_model<T>>& model)
		{
			const auto submodels = model->get_submodels();
			std::for_each(submodels.begin(), submodels.end(), [=](const std::pair<ctti_guid_t, i_binding_model_shared_ptr>& it) {
				if (it.second)
				{
					const auto submodel = std::static_pointer_cast<i_binding_model>(it.second);
					model->remove_submodel(submodel);
					destruct(std::static_pointer_cast<binding_model<T>>(submodel));
				}
			});

			const auto pool = i_binding_model_pool::get_pool(model->instance_guid());
			pool->push_back(model);

			model->remove_from_storage();
			model->reset();
		};

		virtual void init() override
		{

		};

		virtual void reset() override
		{

		};

		template<typename RESULT>
		static std::shared_ptr<std::list<i_binding_model_shared_ptr>> get_storage()
		{
			return i_binding_model_storage::get_storage(RESULT::class_guid());
		};

		void add_to_storage()
		{
			const auto& instance = std::static_pointer_cast<binding_model<T>>(i_binding_model::shared_from_this());
			const auto& storage = i_binding_model_storage::get_storage(instance->instance_guid());
			assert(storage != nullptr);
			storage->push_back(instance);
		};

		void remove_from_storage()
		{
			const auto& instance = std::static_pointer_cast<binding_model<T>>(i_binding_model::shared_from_this());
			const auto& storage = i_binding_model_storage::get_storage(instance->instance_guid());
			assert(storage != nullptr);
			storage->remove(instance);
		};

		void subscribe(const i_binding_property_shared_ptr& prop, ctti_guid_t record_id, const std::string& prop_id)
		{
			const auto instance = i_binding_listener::shared_from_this();
			const auto event_id = binding_event_model_prop::generate_guid(record_id, prop_id);
			prop->set_id(event_id);
			prop->subscribe(instance);
		};

		void on_changed(const i_binding_property_shared_ptr& prop) override
		{
			const auto& instance = std::static_pointer_cast<i_binding_model>(shared_from_this());
			const auto event_id = prop->get_id();
			global_dispatcher::get_instance()->publish<binding_event_model_prop>(event_id, instance, prop);
		};
	};

#define SUBSCRIBE_PROP(__class__, __record_guid__, __prop__) \
this->subscribe(this->record_data->get()->__prop__, __record_guid__, __class__::PROPERTY_ID_##__prop__()); \

};



