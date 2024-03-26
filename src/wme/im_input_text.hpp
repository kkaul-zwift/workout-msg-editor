#pragma once
#include <imgui.h>
#include <span>
#include <string_view>
#include <vector>

namespace wme {
/// \brief Expandable ImGui input text buffer.
class ImInputText {
  public:
	static constexpr std::size_t init_size_v{64};

	auto update(char const* name) -> bool;
	void set_text(std::string_view text);

	auto operator()(char const* name) -> bool { return update(name); }

	[[nodiscard]] auto as_view() const -> std::string_view { return m_buffer.data(); }
	[[nodiscard]] auto as_span() const -> std::span<char const> { return m_buffer; }

	int flags{};

  protected:
	auto on_callback(ImGuiInputTextCallbackData& data) -> int;

	void resize_buffer(ImGuiInputTextCallbackData& data);

	std::vector<char> m_buffer{};
};
} // namespace wme
