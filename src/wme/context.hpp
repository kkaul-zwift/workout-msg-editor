#pragma once
#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <vulkan/vulkan.hpp>
#include <wme/ptr.hpp>
#include <cstdint>
#include <memory>
#include <optional>

namespace wme {
/// \brief Custom deleter for GLFWwindow.
struct WindowDeleter {
	void operator()(Ptr<GLFWwindow> window) const noexcept {
		glfwDestroyWindow(window);
		glfwTerminate();
	}
};

/// \brief A unique GLFW window.
using UniqueWindow = std::unique_ptr<GLFWwindow, WindowDeleter>;

/// \brief Context and renderer.
///
/// Owns UniqueWindow, Vulkan device, and Dear ImGui.
class Context {
  public:
	/// \brief Create a UniqueWindow given its size and title.
	/// \param size Window size.
	/// \param title Window title.
	/// \returns UniqueWindow if successful.
	/// \throws Error on failure.
	[[nodiscard]] static auto create_window(ImVec2 size, char const* title) -> UniqueWindow;

	/// \brief Constructor.
	/// \param window Unique GLFW window to take ownership of.
	/// \throws Error on failure.
	explicit Context(UniqueWindow window) noexcept(false);

	/// \brief Start next frame.
	/// \returns true unless the GLFW window close flag has been set.
	auto next_frame() -> bool;

	/// \brief Render the frame.
	/// \param clear Clear colour for the render pass.
	/// \pre next_frame() must have returned true.
	void render(ImVec4 const& clear = {});

	/// \brief Close the context and its window.
	///
	/// Sets the GLFW window close flag.
	void close();

	[[nodiscard]] auto get_window() const -> Ptr<GLFWwindow> { return m_window.get(); }
	[[nodiscard]] auto get_framebuffer_size() const -> ImVec2;

  private:
	static constexpr auto max_timeout_v{std::numeric_limits<std::uint64_t>::max()};

	void create_instance();
	void create_surface();
	void select_gpu();
	void create_device();

	void create_swapchain();
	void create_sync();
	void create_render_pass();
	void create_dear_imgui();

	auto acquire_next_image() -> std::optional<vk::ImageView>;
	void submit_and_present();

	auto recreate_swapchain(vk::Extent2D framebuffer) -> bool;
	[[nodiscard]] auto create_framebuffer(vk::ImageView render_target) const -> vk::UniqueFramebuffer;

	struct DeviceBlocker {
		vk::Device device{};
	};

	struct Gpu {
		vk::PhysicalDevice device{};
		vk::PhysicalDeviceProperties properties{};
		std::uint32_t queue_family{};
	};

	struct DearImGui {
		enum class State : int { eBegin, eEnd };

		vk::UniqueDescriptorPool descriptor_pool{};
		State state{};

		void begin_frame();
		void end_frame();
	};

	struct Deleter {
		void operator()(Ptr<GLFWwindow> window) const noexcept;
		void operator()(Ptr<DeviceBlocker> blocker) const noexcept;
		void operator()(Ptr<DearImGui>) const noexcept;
	};

	struct Command {
		vk::UniqueCommandPool pool{};
		vk::CommandBuffer buffer{};

		[[nodiscard]] static auto make(vk::Device device, std::uint32_t queue_family) -> Command;
	};

	struct Swapchain {
		vk::SwapchainCreateInfoKHR create_info{};
		vk::UniqueSwapchainKHR swapchain{};
		std::vector<vk::Image> images{};
		std::vector<vk::UniqueImageView> image_views{};
		std::optional<std::uint32_t> image_index{};
	};

	struct Sync {
		Command command{};
		vk::UniqueSemaphore draw{};
		vk::UniqueSemaphore present{};
		vk::UniqueFence drawn{};
	};

	UniqueWindow m_window{};
	vk::UniqueInstance m_instance{};
	vk::UniqueSurfaceKHR m_surface{};
	Gpu m_gpu{};
	std::uint32_t m_queue_family{};
	vk::UniqueDevice m_device{};
	vk::Queue m_queue{};

	Swapchain m_swapchain{};
	Sync m_sync{};
	vk::UniqueRenderPass m_render_pass{};
	vk::UniqueFramebuffer m_framebuffer{};
	std::unique_ptr<DearImGui, Deleter> m_imgui{};

	std::unique_ptr<DeviceBlocker, Deleter> m_blocker{};
};
} // namespace wme
