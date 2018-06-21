#include "graphics_context.h"
#include "ogl_window.h"

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

		struct queue_family 
		{
			i32 m_graphics_family = -1;
			i32 m_present_family = -1;

			bool is_complete() const
			{
				return m_graphics_family >= 0 && m_present_family >= 0;
			};
		};

		struct swap_chain_support_details 
		{
			VkSurfaceCapabilitiesKHR m_capabilities;
			std::vector<VkSurfaceFormatKHR> m_formats;
			std::vector<VkPresentModeKHR> m_present_modes;
		};

	protected:

		HDC m_hdc;
		
		VkSurfaceKHR m_vk_surface = VK_NULL_HANDLE;
		
		VkInstance m_vk_instance = VK_NULL_HANDLE;

		VkDebugReportCallbackEXT m_vk_debug_callback = VK_NULL_HANDLE;

		VkPhysicalDevice m_vk_physical_device = VK_NULL_HANDLE;
		VkDevice m_vk_logical_device = VK_NULL_HANDLE;

		VkQueue m_vk_graphics_queue = VK_NULL_HANDLE;
		VkQueue m_vk_present_queue = VK_NULL_HANDLE;

		VkSwapchainKHR m_vk_swap_chain = VK_NULL_HANDLE;

		std::vector<VkImage> m_vk_swap_chain_images;
		VkFormat m_vk_swap_chain_image_format;
		VkExtent2D m_vk_swap_chain_extent;

		std::vector<VkImageView> m_vk_swap_chain_image_views;

		std::vector<const char*> m_instance_extensions = {
		};

		std::vector<const char*> m_device_extensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
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

		void create_surface(const std::shared_ptr<ogl_window>& window);
		void create_device();
		void create_swap_chain(const std::shared_ptr<ogl_window>& window);
		void create_image_views();
		void create_debug_callback();
		void create_pipeline();

		queue_family get_queue_family(VkPhysicalDevice device);

		swap_chain_support_details query_swap_chain_support(VkPhysicalDevice device);

		VkSurfaceFormatKHR choose_swap_surface_format(const std::vector<VkSurfaceFormatKHR>& formats);

		VkPresentModeKHR choose_swap_present_mode(const std::vector<VkPresentModeKHR> present_modes);

		VkExtent2D choose_swap_extent(const VkSurfaceCapabilitiesKHR& capabilities, const std::shared_ptr<ogl_window>& window);

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

		create_surface(window);
		create_debug_callback();
		create_device();
		create_swap_chain(window);
		create_image_views();
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
			vkDestroyImageView(m_vk_logical_device, image_view, nullptr);
		}

		vkDestroySwapchainKHR(m_vk_logical_device, m_vk_swap_chain, nullptr);
		vkDestroyDevice(m_vk_logical_device, nullptr);
		vkDestroySurfaceKHR(m_vk_instance, m_vk_surface, nullptr);
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

	void graphics_context_win32_vk::create_surface(const std::shared_ptr<ogl_window>& window)
	{
		VkWin32SurfaceCreateInfoKHR surface_create_info = {};
		surface_create_info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		surface_create_info.hwnd = window->get_hwnd();
		surface_create_info.hinstance = GetModuleHandle(nullptr);

		auto CreateWin32SurfaceKHR = (PFN_vkCreateWin32SurfaceKHR)vkGetInstanceProcAddr(m_vk_instance, "vkCreateWin32SurfaceKHR");
		if (CreateWin32SurfaceKHR)
		{
			VkResult result = CreateWin32SurfaceKHR(m_vk_instance, &surface_create_info, nullptr, &m_vk_surface);
			assert(result == VK_SUCCESS);
		}
		else
		{
			assert(false);
		}
	}

	void graphics_context_win32_vk::create_device()
	{
		uint32_t devices_count = 0;
		vkEnumeratePhysicalDevices(m_vk_instance, &devices_count, nullptr);
		assert(devices_count != 0);

		std::vector<VkPhysicalDevice> devices(devices_count);
		vkEnumeratePhysicalDevices(m_vk_instance, &devices_count, devices.data());

		VkPhysicalDeviceProperties device_properties;
		VkPhysicalDeviceFeatures device_features;

		for (const auto& device : devices) {
			vkGetPhysicalDeviceProperties(device, &device_properties);
			vkGetPhysicalDeviceFeatures(device, &device_features);

			if (device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && device_features.geometryShader) {
				m_vk_physical_device = device;
				break;
			}
		}

		queue_family queue_family = get_queue_family(m_vk_physical_device);

		std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
		std::set<int> unique_queue_families = { queue_family.m_graphics_family, queue_family.m_present_family };

		float queue_priority = 1.0f;
		for (int queueFamily : unique_queue_families) {
			VkDeviceQueueCreateInfo queue_create_info = {};
			queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queue_create_info.queueFamilyIndex = queueFamily;
			queue_create_info.queueCount = 1;
			queue_create_info.pQueuePriorities = &queue_priority;
			queue_create_infos.push_back(queue_create_info);
		}

		VkDeviceCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		create_info.queueCreateInfoCount = static_cast<ui32>(queue_create_infos.size());
		create_info.pQueueCreateInfos = queue_create_infos.data();
		create_info.pEnabledFeatures = &device_features;

		create_info.enabledExtensionCount = static_cast<uint32_t>(m_device_extensions.size());
		create_info.ppEnabledExtensionNames = m_device_extensions.data();

		VkResult result = vkCreateDevice(m_vk_physical_device, &create_info, nullptr, &m_vk_logical_device);
		assert(result == VK_SUCCESS);

		vkGetDeviceQueue(m_vk_logical_device, queue_family.m_graphics_family, 0, &m_vk_graphics_queue);
		vkGetDeviceQueue(m_vk_logical_device, queue_family.m_present_family, 0, &m_vk_present_queue);
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

	void graphics_context_win32_vk::create_swap_chain(const std::shared_ptr<ogl_window>& window)
	{
		swap_chain_support_details swap_chain_support = query_swap_chain_support(m_vk_physical_device);

		VkSurfaceFormatKHR surface_format = choose_swap_surface_format(swap_chain_support.m_formats);
		VkPresentModeKHR present_mode = choose_swap_present_mode(swap_chain_support.m_present_modes);
		VkExtent2D extent = choose_swap_extent(swap_chain_support.m_capabilities, window);

		uint32_t images_count = swap_chain_support.m_capabilities.minImageCount + 1;
		if (swap_chain_support.m_capabilities.maxImageCount > 0 && images_count > swap_chain_support.m_capabilities.maxImageCount)
		{
			images_count = swap_chain_support.m_capabilities.maxImageCount;
		}

		VkSwapchainCreateInfoKHR create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		create_info.surface = m_vk_surface;

		create_info.minImageCount = images_count;
		create_info.imageFormat = surface_format.format;
		create_info.imageColorSpace = surface_format.colorSpace;
		create_info.imageExtent = extent;
		create_info.imageArrayLayers = 1;
		create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		queue_family queue_family = get_queue_family(m_vk_physical_device);

		if (queue_family.m_graphics_family != queue_family.m_present_family) 
		{
			create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			create_info.queueFamilyIndexCount = 2;
			ui32 queue_families[] = { (ui32)queue_family.m_graphics_family, (ui32)queue_family.m_present_family };
			create_info.pQueueFamilyIndices = queue_families;
		}
		else
		{
			create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			create_info.queueFamilyIndexCount = 1;
			ui32 queue_families[] = { (ui32)queue_family.m_graphics_family};
			create_info.pQueueFamilyIndices = queue_families;
		}

		create_info.preTransform = swap_chain_support.m_capabilities.currentTransform;
		create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		create_info.presentMode = present_mode;
		create_info.clipped = VK_TRUE;

		create_info.oldSwapchain = VK_NULL_HANDLE;

		VkResult result = vkCreateSwapchainKHR(m_vk_logical_device, &create_info, nullptr, &m_vk_swap_chain);
		assert(result == VK_SUCCESS);

		vkGetSwapchainImagesKHR(m_vk_logical_device, m_vk_swap_chain, &images_count, nullptr);
		m_vk_swap_chain_images.resize(images_count);
		vkGetSwapchainImagesKHR(m_vk_logical_device, m_vk_swap_chain, &images_count, m_vk_swap_chain_images.data());

		m_vk_swap_chain_image_format = surface_format.format;
		m_vk_swap_chain_extent = extent;
	}

	void graphics_context_win32_vk::create_image_views()
	{
		m_vk_swap_chain_image_views.resize(m_vk_swap_chain_images.size());

		for (size_t i = 0; i < m_vk_swap_chain_images.size(); i++) {
			VkImageViewCreateInfo create_info = {};
			create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			create_info.image = m_vk_swap_chain_images[i];
			create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
			create_info.format = m_vk_swap_chain_image_format;
			create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
			create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			create_info.subresourceRange.baseMipLevel = 0;
			create_info.subresourceRange.levelCount = 1;
			create_info.subresourceRange.baseArrayLayer = 0;
			create_info.subresourceRange.layerCount = 1;

			VkResult result = vkCreateImageView(m_vk_logical_device, &create_info, nullptr, &m_vk_swap_chain_image_views[i]);
			assert(result == VK_SUCCESS);
		}
	}

	void graphics_context_win32_vk::create_pipeline()
	{

	}

	graphics_context_win32_vk::queue_family graphics_context_win32_vk::get_queue_family(VkPhysicalDevice device)
	{
		queue_family result;
		ui32 queue_familie_count = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_familie_count, nullptr);

		std::vector<VkQueueFamilyProperties> queue_families(queue_familie_count);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_familie_count, queue_families.data());

		i32 index = 0;
		for (const auto& queue_family : queue_families) 
		{
			if (queue_family.queueCount > 0 && queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				result.m_graphics_family = index;
			}

			if (m_vk_surface)
			{
				VkBool32 present_support = false;
				vkGetPhysicalDeviceSurfaceSupportKHR(device, index, m_vk_surface, &present_support);

				if (queue_family.queueCount > 0 && present_support) 
				{
					result.m_present_family = index;
				}
			} 
			else
			{
				assert(false);
			}

			if (result.is_complete()) 
			{
				break;
			}
			index++;
		}
		return result;
	}

	graphics_context_win32_vk::swap_chain_support_details graphics_context_win32_vk::query_swap_chain_support(VkPhysicalDevice device) {
		swap_chain_support_details details;

		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, m_vk_surface, &details.m_capabilities);

		uint32_t formats_count;
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_vk_surface, &formats_count, nullptr);

		if (formats_count != 0) {
			details.m_formats.resize(formats_count);
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_vk_surface, &formats_count, details.m_formats.data());
		}

		uint32_t present_modes_count;
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_vk_surface, &present_modes_count, nullptr);

		if (present_modes_count != 0) {
			details.m_present_modes.resize(present_modes_count);
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_vk_surface, &present_modes_count, details.m_present_modes.data());
		}

		return details;
	}

	VkSurfaceFormatKHR graphics_context_win32_vk::choose_swap_surface_format(const std::vector<VkSurfaceFormatKHR>& formats)
	{
		if (formats.size() == 1 && formats[0].format == VK_FORMAT_UNDEFINED) 
		{
			return { VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
		}

		for (const auto& format : formats) 
		{
			if (format.format == VK_FORMAT_B8G8R8A8_UNORM && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) 
			{
				return format;
			}
		}

		return formats[0];
	}

	VkPresentModeKHR graphics_context_win32_vk::choose_swap_present_mode(const std::vector<VkPresentModeKHR> present_modes)
	{
		VkPresentModeKHR mode = VK_PRESENT_MODE_FIFO_KHR;

		for (const auto& present_mode : present_modes) {
			if (present_mode == VK_PRESENT_MODE_MAILBOX_KHR) 
			{
				return present_mode;
			}
			else if (present_mode == VK_PRESENT_MODE_IMMEDIATE_KHR)
			{
				mode = present_mode;
			}
		}

		return mode;
	}

	VkExtent2D graphics_context_win32_vk::choose_swap_extent(const VkSurfaceCapabilitiesKHR& capabilities, const std::shared_ptr<ogl_window>& window)
	{
		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) 
		{
			return capabilities.currentExtent;
		}
		else
		{
			VkExtent2D actual_extent = { window->get_width(), window->get_height() };

			actual_extent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actual_extent.width));
			actual_extent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actual_extent.height));

			return actual_extent;
		}
	}
}

#endif