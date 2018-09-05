#include "graphics_context.h"
#include "ogl_window.h"
#include "vk_device.h"
#include "vk_surface.h"
#include "vk_swap_chain.h"

#if defined(VULKAN_API)

namespace gb
{
	static VKAPI_ATTR VkBool32 VKAPI_CALL vk_debug_callback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT object_type, uint64_t object,
		size_t location, int32_t code, const char* layer_prefix, const char* msg, void* user_data) {
		std::cerr<<"validation layer: "<<msg<<std::endl;
		return VK_FALSE;
	}

	class graphics_context_win32_vk : public graphics_context
	{
	private:

	protected:

		HDC m_hdc;
		
		VkInstance m_vk_instance = VK_NULL_HANDLE;

		VkDebugReportCallbackEXT m_vk_debug_callback = VK_NULL_HANDLE;

		std::vector<VkImageView> m_vk_swap_chain_image_views;
		std::vector<VkFramebuffer> m_vk_swap_chain_framebuffers;

		std::vector<const char*> m_instance_extensions = {
		};

		std::vector<const char*> m_layers = {
			"VK_LAYER_LUNARG_standard_validation"
		};

	public:

		graphics_context_win32_vk(const std::shared_ptr<ogl_window>& window);
		~graphics_context_win32_vk();

		void* get_context() const;

		void make_current();
		void draw() const;

		void create_debug_callback();
	};

	std::shared_ptr<graphics_context> create_graphics_context_win32_vk(const std::shared_ptr<ogl_window>& window)
	{
		assert(window != nullptr);
		return std::make_shared<graphics_context_win32_vk>(window);
	};

	graphics_context_win32_vk::graphics_context_win32_vk(const std::shared_ptr<ogl_window>& window)
	{
		m_window = window;
		m_hdc = m_window->get_hdc();

		ui32 extension_count = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr);

		std::vector<VkExtensionProperties> extensions(extension_count);
		std::vector<const char*> extensions_str(extension_count);
		vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, extensions.data());

		std::cout << "available vk extensions:" << std::endl;
		for (const auto& extension : extensions)
		{
			std::cout << extension.extensionName << std::endl;
			m_instance_extensions.push_back(extension.extensionName);
		}

		uint32_t layers_count;
		vkEnumerateInstanceLayerProperties(&layers_count, nullptr);

		std::vector<VkLayerProperties> layers(layers_count);
		vkEnumerateInstanceLayerProperties(&layers_count, layers.data());

		std::cout << "available vk layers:" << std::endl;
		for (const auto& layer : layers)
		{
			std::cout << layer.layerName << std::endl;
		}

		VkApplicationInfo app_info = {};
		app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		app_info.pApplicationName = "game-box";
		app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		app_info.pEngineName = "game-box";
		app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		app_info.apiVersion = VK_API_VERSION_1_0;

		VkInstanceCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		create_info.pApplicationInfo = &app_info;

		create_info.enabledExtensionCount = static_cast<ui32>(m_instance_extensions.size());
		create_info.ppEnabledExtensionNames = m_instance_extensions.data();

		create_info.enabledLayerCount = static_cast<ui32>(m_layers.size());
		create_info.ppEnabledLayerNames = m_layers.data();

		VkResult result = vkCreateInstance(&create_info, nullptr, &m_vk_instance);
		assert(result == VK_SUCCESS);

		vk_surface::get_instance()->construct(m_vk_instance, window);
		vk_device::get_instance()->construct(m_vk_instance);
		vk_swap_chain::get_instance()->construct(m_vk_instance, window);
		vk_swap_chain::get_instance()->connect(m_vk_instance);
		vk_device::get_instance()->create_cmd_buffers();
		vk_device::get_instance()->create_frame_buffers(window);
		vk_device::get_instance()->synchronize();

		create_debug_callback();
	}

	graphics_context_win32_vk::~graphics_context_win32_vk()
	{
		auto DestroyDebugReportCallbackEXT = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(m_vk_instance, "vkDestroyDebugReportCallbackEXT");
		if (DestroyDebugReportCallbackEXT != nullptr) 
		{
			DestroyDebugReportCallbackEXT(m_vk_instance, m_vk_debug_callback, nullptr);
		}

		for (auto image_view : m_vk_swap_chain_image_views) 
		{
			vkDestroyImageView(vk_device::get_instance()->get_logical_device(), image_view, nullptr);
		}

		vkDestroySwapchainKHR(vk_device::get_instance()->get_logical_device(), vk_swap_chain::get_instance()->get(), nullptr);
		vkDestroyDevice(vk_device::get_instance()->get_logical_device(), nullptr);
		vkDestroySurfaceKHR(m_vk_instance, vk_surface::get_instance()->get(), nullptr);
		vkDestroyInstance(m_vk_instance, nullptr);
	}

	void* graphics_context_win32_vk::get_context() const
	{
		return nullptr;
	}

	void graphics_context_win32_vk::make_current()
	{
		VkSemaphore present_complete_semaphore = vk_device::get_instance()->get_present_complete_semaphore();
		VkSemaphore render_complete_semaphore = vk_device::get_instance()->get_render_complete_semaphore();

		ui32 current_image_index = 0;
		VkResult result = vk_swap_chain::get_instance()->acquire_next_image(present_complete_semaphore, &current_image_index);
		assert(result == VK_SUCCESS);
		vk_device::get_instance()->set_current_image_index(current_image_index);

		VkFence wait_fence = vk_device::get_instance()->get_wait_fence(current_image_index);
		result = vkWaitForFences(vk_device::get_instance()->get_logical_device(), 1, &wait_fence, VK_TRUE, UINT64_MAX);
		assert(result == VK_SUCCESS);

		result = vkResetFences(vk_device::get_instance()->get_logical_device(), 1, &wait_fence);
		assert(result == VK_SUCCESS);
	}

	void graphics_context_win32_vk::draw() const
	{
		VkSemaphore present_complete_semaphore = vk_device::get_instance()->get_present_complete_semaphore();
		VkSemaphore render_complete_semaphore = vk_device::get_instance()->get_render_complete_semaphore();

		ui32 current_image_index = vk_device::get_instance()->get_current_image_index();
		VkCommandBuffer draw_cmd_buffer = vk_device::get_instance()->get_draw_cmd_buffer(current_image_index);
		VkPipelineStageFlags wait_stage_mask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

		VkSubmitInfo submit_info = {};
		submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submit_info.pWaitDstStageMask = &wait_stage_mask;
		submit_info.pWaitSemaphores = &present_complete_semaphore;
		submit_info.waitSemaphoreCount = 1;
		submit_info.pSignalSemaphores = &render_complete_semaphore;
		submit_info.signalSemaphoreCount = 1;
		submit_info.pCommandBuffers = &draw_cmd_buffer;
		submit_info.commandBufferCount = 1;

		VkQueue graphics_queue = vk_device::get_instance()->get_graphics_queue();
		VkFence wait_fence = vk_device::get_instance()->get_wait_fence(current_image_index);

		VkResult result = vkQueueSubmit(graphics_queue, 1, &submit_info, wait_fence);
		assert(result == VK_SUCCESS);

		result = vk_swap_chain::get_instance()->queue_present(graphics_queue, current_image_index, render_complete_semaphore);
		assert(result == VK_SUCCESS);
	}

	void graphics_context_win32_vk::create_debug_callback()
	{
		VkDebugReportCallbackCreateInfoEXT callback_create_info = {};
		callback_create_info.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
		callback_create_info.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
		callback_create_info.pfnCallback = vk_debug_callback;

		auto CreateDebugReportCallbackEXT = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(m_vk_instance, "vkCreateDebugReportCallbackEXT");
		if (CreateDebugReportCallbackEXT != nullptr)
		{
			VkResult result = CreateDebugReportCallbackEXT(m_vk_instance, &callback_create_info, nullptr, &m_vk_debug_callback);
			assert(result == VK_SUCCESS);
		}
		else
		{
			assert(false);
		}
	}
}

#endif