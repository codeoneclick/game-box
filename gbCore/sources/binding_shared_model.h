#pragma once

#include "main_headers.h"
#include "declarations.h"
#include "i_binding_model.h"
#include "i_binding_record.h"

namespace gb
{
	class binding_shared_record : public i_binding_record
	{
	public:

		CTTI_CLASS_GUID(binding_shared_record, i_binding_record::g_guids_container, override)

		PROP(public, binding_shared_record, version, i32, 0)
	};


	class binding_shared_model : public binding_model<binding_shared_record>
	{
	private:

		static std::shared_ptr<binding_shared_model> m_instance;

		std::unordered_map<guid_t, std::shared_ptr<std::list<i_binding_model_shared_ptr>>> m_pools;
		std::unordered_map<guid_t, std::shared_ptr<std::list<i_binding_model_shared_ptr>>> m_storages;

	public:

		binding_shared_model();
		~binding_shared_model();

		static std::shared_ptr<binding_shared_model> get_instance();

		CTTI_CLASS_GUID(binding_shared_model, i_binding_model::g_guids_container, override)

		void init() override;

		void serialize() override;
		void deserialize(const void* data) override;

		template<typename T>
		void register_storage_and_pool()
		{
			assert(m_storages[T::class_guid()] == nullptr);
			m_storages[T::class_guid()] = std::make_shared<std::list<i_binding_model_shared_ptr>>();

			assert(m_pools[T::class_guid()] == nullptr);
			m_pools[T::class_guid()] = std::make_shared<std::list<i_binding_model_shared_ptr>>();
		};

		void register_storage_and_pool(guid_t guid)
		{
			assert(m_storages[guid] == nullptr);
			m_storages[guid] = std::make_shared<std::list<i_binding_model_shared_ptr>>();

			assert(m_pools[guid] == nullptr);
			m_pools[guid] = std::make_shared<std::list<i_binding_model_shared_ptr>>();
		}

		template<typename T>
		void unregister_storage_and_pool()
		{
			assert(m_storages[T::class_guid()] != nullptr);
			m_storages[T::class_guid()]->clear();
			m_storages[T::class_guid()] = nullptr;

			assert(m_pools[T::class_guid()] == nullptr);
			m_pools[T::class_guid()]->clear();
			m_pools[T::class_guid()] = nullptr;
		};

		void unregister_all_storages_and_pools();

		bool is_storage_and_pool_registered(guid_t guid) const;

		template<typename T>
		std::shared_ptr<std::list<i_binding_model_shared_ptr>> get_storage()
		{
			return get_storage(T::class_guid());
		};

		template<typename T>
		std::shared_ptr<std::list<i_binding_model_shared_ptr>> get_pool()
		{
			return get_pool(T::class_guid());
		};

		std::shared_ptr<std::list<i_binding_model_shared_ptr>> get_storage(guid_t guid);
		std::shared_ptr<std::list<i_binding_model_shared_ptr>> get_pool(guid_t guid);

		template<typename TModel>
		void deserialize(const void* data)
		{
			const auto it = m_submodels.find(TModel::class_guid());
			if (it != m_submodels.end())
			{
				it->second->deserialize(data);
			}
		};
	};
};

