#include <fmt/format.h>
#include <wme/app.hpp>
#include <wme/build_version.hpp>
#include <wme/log.hpp>
#include <filesystem>

namespace wme {
namespace fs = std::filesystem;

App::App(GLFWwindow& window) {
	static auto const self = [](Ptr<GLFWwindow> window) -> App& {
		return *static_cast<App*>(glfwGetWindowUserPointer(window));
	};

	glfwSetWindowUserPointer(&window, this);
	glfwSetDropCallback(&window, [](Ptr<GLFWwindow> window, int count, char const** paths) {
		self(window).on_drop({paths, static_cast<std::size_t>(count)});
	});
	glfwSetWindowCloseCallback(&window, [](Ptr<GLFWwindow> window) { self(window).handle_close(); });
	glfwSetKeyCallback(&window, [](Ptr<GLFWwindow> window, int key, int /*scancode*/, int action, int mods) {
		self(window).on_key(key, action, mods);
	});
}

auto App::run(Context context) -> int {
	m_context = std::move(context);
	set_mode();

	try {
		while (m_context->next_frame()) {
			tick();
			m_context->render();
		}
	} catch (std::exception const& e) {
		log::error("PANIC: {}", e.what());
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

void App::on_drop(std::span<char const* const> paths) {
	for (auto const* path : paths) {
		try {
			load_workout(path);
		} catch (std::exception const& e) {
			auto const filename = fs::path{path}.filename().string();
			log::error("'{}': {}", filename, e.what());
		}
	}
}

void App::on_key(int const key, int const action, int const mods) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE && mods == 0) {
		if (m_unsaved) {
			m_confirm_quit = true;
		} else {
			glfwSetWindowShouldClose(m_context->get_window(), GLFW_TRUE);
		}
	}
}

void App::handle_close() {
	if (!m_unsaved) { return; }

	m_confirm_quit = true;
	glfwSetWindowShouldClose(m_context->get_window(), GLFW_FALSE);
}

void App::tick() {
	ImGui::SetNextWindowSize(m_context->get_framebuffer_size());
	ImGui::SetNextWindowPos({});
	static constexpr auto flags_v =
		ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar;
	if (ImGui::Begin(m_filename.c_str(), nullptr, flags_v)) { inspect(); }
	ImGui::End();

	if (m_confirm_quit) {
		if (!ImGui::IsPopupOpen("Confirm Quit")) { ImGui::OpenPopup("Confirm Quit"); }
		m_confirm_quit = false;
	}

	auto const center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2{0.5f, 0.5f});
	if (ImGui::BeginPopupModal("Confirm Quit", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::Text("Are you sure you want to quit?");
		ImGui::Text("There are unsaved changes!");
		if (ImGui::Button("Yes")) { m_context->close(); }
		ImGui::SameLine();
		if (ImGui::Button("No")) { ImGui::CloseCurrentPopup(); }
		ImGui::EndPopup();
	}
}

void App::update_title() {
	m_filename = fs::path{m_path}.filename().string();
	auto const title = fmt::format("{}{} [v{}]", m_unsaved ? "*" : "", m_filename, build_version_v);
	glfwSetWindowTitle(m_context->get_window(), title.c_str());
}

void App::load_workout(std::string path) noexcept(false) {
	m_workout.load_from_file(path.c_str());
	m_path = std::move(path);
	m_editor.emplace(m_workout);
	m_unsaved = false;
	update_title();
}

void App::set_mode() {
	using enum Mode;
	switch (m_mode) {
	case eLight: ImGui::StyleColorsLight(); break;
	case eDark: ImGui::StyleColorsDark(); break;
	default: break;
	}
}

void App::inspect() {
	if (m_editor) { ImGui::Text("%s", m_workout.name); }

	ImGui::BeginDisabled(!m_unsaved);
	if (ImGui::Button("Save")) {
		if (m_workout.save_to_file(m_path.c_str())) {
			m_unsaved = false;
			update_title();
		}
	}
	ImGui::EndDisabled();
	ImGui::SameLine();
	ImGui::SetNextItemWidth(120.0f);
	if (ImGui::BeginCombo("##Mode", mode_name_v[static_cast<std::size_t>(m_mode)])) {
		for (std::size_t i = 0; i < mode_name_v.size(); ++i) {
			if (ImGui::Selectable(mode_name_v.at(i))) {
				m_mode = static_cast<Mode>(i);
				set_mode();
			}
		}
		ImGui::EndCombo();
	}

	if (ImGui::BeginChild("Text Events", {}, ImGuiChildFlags_Border)) {
		if (m_editor) {
			auto const is_modified = m_editor->inspect();
			if (is_modified && !m_unsaved) {
				m_unsaved = true;
				update_title();
			}
		} else {
			ImGui::Text("Drag a workout file to load it");
		}
	}
	ImGui::EndChild();
}
} // namespace wme
