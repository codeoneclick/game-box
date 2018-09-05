#include "binding_shared_model.h"

namespace gb
{
	std::shared_ptr<binding_shared_model> binding_shared_model::m_instance = nullptr;

	binding_shared_model::binding_shared_model()
	{

	}

	binding_shared_model::~binding_shared_model()
	{

	}

	std::shared_ptr<binding_shared_model> binding_shared_model::get_instance()
	{
		if (m_instance == nullptr)
		{
			m_instance = std::make_shared<binding_shared_model>();
		}
		return m_instance;
	}

	void binding_shared_model::init()
	{
		binding_model<binding_shared_record>::init();

		SUBSCRIBE_PROP(binding_shared_record, binding_shared_record::class_guid(), version)
	}

	void binding_shared_model::serialize()
	{

	}

	void binding_shared_model::deserialize(const void* data)
	{

	}
	void binding_shared_model::unregister_all_storages_and_pools()
	{
		for (const auto it : m_storages)
		{
			if (it.second)
			{
				it.second->clear();
			}
		}
		m_storages.clear();

		for (const auto it : m_pools)
		{
			if (it.second)
			{
				it.second->clear();
			}
		}
		m_pools.clear();
	}

	bool binding_shared_model::is_storage_and_pool_registered(guid_t guid) const
	{
		bool result = false;
		const auto storage_it = m_storages.find(guid);
		const auto pool_it = m_pools.find(guid);
		if (storage_it != m_storages.end() && pool_it != m_pools.end())
		{
			result = true;
		}
		return result;
	}

	std::shared_ptr<std::list<i_binding_model_shared_ptr>> binding_shared_model::get_storage(guid_t guid)
	{
		return m_storages[guid];
	}

	std::shared_ptr<std::list<i_binding_model_shared_ptr>> binding_shared_model::get_pool(guid_t guid)
	{
		return m_pools[guid];
	}
}