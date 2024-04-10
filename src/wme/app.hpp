#pragma once
#include <wme/context.hpp>
#include <wme/options.hpp>
#include <wme/workout_edit.hpp>
#include <array>
#include <optional>

namespace wme {
class App {
  public:
	explicit App(GLFWwindow& window);

	auto run(Context context) -> int;

  private:
	enum class Mode { eLight, eDark };

	void on_drop(std::span<char const* const> paths);
	void on_key(int key, int action, int mods);
	void handle_close();
	void tick();

	void update_title();
	void load_workout(std::string path) noexcept(false);
	void set_mode();

	void inspect();
	void save_button();
	void mode_combo(float width);
	void workout_child_window();

	std::optional<Context> m_context{};
	Mode m_mode{Mode::eLight};
	Options m_options{};

	std::string m_path{};
	std::string m_filename{"[No Workout Loaded]"};
	Workout m_workout{};
	std::optional<WorkoutEdit> m_editor{};
	bool m_unsaved{};
	bool m_confirm_quit{};
};
} // namespace wme
