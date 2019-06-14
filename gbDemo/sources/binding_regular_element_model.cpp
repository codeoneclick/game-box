#include "binding_regular_element_model.h"

namespace game
{
	bool binding_regular_element_model::is_base_of(stti_guid_t base_guid)
	{
		return base_guid == binding_sub_element_model<binding_sub_element_record>::class_guid();
	}
}
