#pragma once
#include <imgui.h>

namespace wme {
[[nodiscard]] inline auto red_button(char const* label) -> bool {
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{1.0f, 0.0f, 0.0f, 1.0f});
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{1.0f, 1.0f, 1.0f, 1.0f});
	auto const ret = ImGui::Button(label);
	ImGui::PopStyleColor(2);
	return ret;
}
} // namespace wme
