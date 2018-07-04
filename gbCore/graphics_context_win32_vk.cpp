#include "graphics_context.h"
#include "ogl_window.h"
#include "vk_device.h"
#include "vk_surface.h"
#include "vk_swap_chain.h"

#if defined(__WINOS__) && !defined(__NO_RENDER__)

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

		VkRenderPass m_vk_render_pass = VK_NULL_HANDLE;
		VkPipelineLayout m_vk_pipeline_layout = VK_NULL_HANDLE;

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

		void create_image_views();
		void create_debug_callback();
		void create_pipeline();
		void create_render_pass();
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

		create_debug_callback();
		create_image_views();
		create_render_pass();
		create_pipeline();
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

	}

	void graphics_context_win32_vk::draw() const
	{
		
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

	void graphics_context_win32_vk::create_image_views()
	{
		m_vk_swap_chain_image_views.resize(vk_swap_chain::get_instance()->get_vk_swap_chain_images().size());

		for (size_t i = 0; i < vk_swap_chain::get_instance()->get_vk_swap_chain_images().size(); i++) {
			VkImageViewCreateInfo create_info = {};
			create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			create_info.image = vk_swap_chain::get_instance()->get_vk_swap_chain_images()[i];
			create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
			create_info.format = vk_swap_chain::get_instance()->get_vk_swap_chain_image_format();
			create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
			create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			create_info.subresourceRange.baseMipLevel = 0;
			create_info.subresourceRange.levelCount = 1;
			create_info.subresourceRange.baseArrayLayer = 0;
			create_info.subresourceRange.layerCount = 1;

			VkResult result = vkCreateImageView(vk_device::get_instance()->get_logical_device(), &create_info, nullptr, &m_vk_swap_chain_image_views[i]);
			assert(result == VK_SUCCESS);
		}
	}

	void graphics_context_win32_vk::create_pipeline()
	{
		VkPipelineLayoutCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		create_info.setLayoutCount = 0;
		create_info.pushConstantRangeCount = 0;

		VkResult result = vkCreatePipelineLayout(vk_device::get_instance()->get_logical_device(), &create_info, nullptr, &m_vk_pipeline_layout);
		assert(result == VK_SUCCESS);
	}

	void graphics_context_win32_vk::create_render_pass()
	{
		VkAttachmentDescription color_attachment = {};
		color_attachment.format = vk_swap_chain::get_instance()->get_vk_swap_chain_image_format();
		color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
		color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference color_attachment_ref = {};
		color_attachment_ref.attachment = 0;
		color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDescription sub_pass = {};
		sub_pass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		sub_pass.colorAttachmentCount = 1;
		sub_pass.pColorAttachments = &color_attachment_ref;

		VkRenderPassCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		create_info.attachmentCount = 1;
		create_info.pAttachments = &color_attachment;
		create_info.subpassCount = 1;
		create_info.pSubpasses = &sub_pass;

		VkResult result = vkCreateRenderPass(vk_device::get_instance()->get_logical_device(), &create_info, nullptr, &m_vk_render_pass);
		assert(result == VK_SUCCESS);
	}
}

#endif