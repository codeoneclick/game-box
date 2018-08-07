#include "vk_uniform_value_buffer.h"
#include "vk_utils.h"

namespace gb
{
	vk_uniform_value_buffer::vk_uniform_value_buffer(ui32 size)
	{
		m_staging_buffer = std::make_shared<vk_buffer>();
		VkResult result = vk_utils::create_buffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_staging_buffer, size);
		assert(result == VK_SUCCESS);

		m_main_buffer = std::make_shared<vk_buffer>();
		result = vk_utils::create_buffer(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_main_buffer, size);
		assert(result == VK_SUCCESS);
	}

	vk_uniform_value_buffer::~vk_uniform_value_buffer()
	{

	}

	std::shared_ptr<vk_buffer> vk_uniform_value_buffer::get_main_buffer() const
	{
		return m_main_buffer;
	}

	std::shared_ptr<vk_buffer> vk_uniform_value_buffer::get_staging_buffer() const
	{
		return m_staging_buffer;
	}

	void vk_uniform_value_buffer::apply(void *data)
	{
		m_staging_buffer->map(m_staging_buffer->get_size(), 0);
		m_staging_buffer->copy_to(data, m_staging_buffer->get_size());
		m_staging_buffer->unmap();

		vk_utils::copy_buffers(m_staging_buffer, m_main_buffer);
	}
}