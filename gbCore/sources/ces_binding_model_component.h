//
//  ces_box2d_world_component.h
//  gbCore
//
//  Created by serhii serhiiv on 11/1/16.
//  Copyright � 2016 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_base_component.h"
#include "i_binding_model.h"

namespace gb
{
	class ces_binding_model_component : public ces_base_component
	{
	private:

		i_binding_model_shared_ptr m_model = nullptr;

	protected:

	public:

		STTI_CLASS_GUID(ces_binding_model_component, ces_base_component::g_guids_container, override);
		ces_binding_model_component();
		~ces_binding_model_component();

		void set_model(const i_binding_model_shared_ptr& model);

		template<typename T>
		std::shared_ptr<T> get_model_as() const
		{
			return std::static_pointer_cast<T>(m_model);
		};
	};
};
