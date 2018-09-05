#include "i_binding_model.h"
#include "binding_shared_model.h"

namespace gb
{
	std::set<guid_t> i_binding_model::g_guids_container;

	i_binding_model::i_binding_model()
	{

	}

	std::shared_ptr<std::list<i_binding_model_shared_ptr>> i_binding_model_storage::get_storage(guid_t guid)
	{
		return binding_shared_model::get_instance()->get_storage(guid);
	}

	bool i_binding_model_storage::is_storage_registered(guid_t guid)
	{
		return binding_shared_model::get_instance()->is_storage_and_pool_registered(guid);
	}

	void i_binding_model_storage::register_storage(guid_t guid)
	{
		binding_shared_model::get_instance()->register_storage_and_pool(guid);
	}

	std::shared_ptr<std::list<i_binding_model_shared_ptr>> i_binding_model_pool::get_pool(guid_t guid)
	{
		return binding_shared_model::get_instance()->get_pool(guid);
	}

	bool i_binding_model_pool::is_pool_registered(guid_t guid)
	{
		return binding_shared_model::get_instance()->is_storage_and_pool_registered(guid);
	}

	void i_binding_model_pool::register_pool(guid_t guid)
	{
		binding_shared_model::get_instance()->register_storage_and_pool(guid);
	}
}