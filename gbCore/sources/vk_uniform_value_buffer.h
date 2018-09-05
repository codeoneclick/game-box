#pragma once

#if defined(VULKAN_API)

#include "vk_buffer.h"

namespace gb
{
	class vk_uniform_value_buffer
	{
	private:

		std::shared_ptr<vk_buffer> m_main_buffer = nullptr;
		std::shared_ptr<vk_buffer> m_staging_buffer = nullptr;

	protected:

	public:

		vk_uniform_value_buffer(ui32 size);
		~vk_uniform_value_buffer();

		std::shared_ptr<vk_buffer> get_main_buffer() const;
		std::shared_ptr<vk_buffer> get_staging_buffer() const;

		void apply(void *data);
	};
};

#endif