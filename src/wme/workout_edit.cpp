#include <wme/fixed_string.hpp>
#include <wme/workout_edit.hpp>

namespace wme {
using namespace std::chrono_literals;

namespace {
[[nodiscard]] auto format_time(std::chrono::seconds seconds) {
	auto minutes = std::chrono::minutes{};
	if (seconds > 60s) {
		minutes = std::chrono::duration_cast<std::chrono::minutes>(seconds);
		seconds -= minutes;
	}
	return FixedString{"{:02}:{:02}", minutes.count(), seconds.count()};
}
} // namespace

WorkoutEdit::WorkoutEdit(Workout const& workout) : m_document(workout.document) {
	m_segments.reserve(workout.segments.size());
	for (auto const& in_segment : workout.segments) {
		auto out_segment = SegmentEdit{};
		out_segment.display_name = fmt::format("{}. {}", in_segment.index, in_segment.name);
		for (auto const& in_event : in_segment.text_events) { out_segment.text_events.emplace_back(in_event); }
		m_segments.push_back(std::move(out_segment));
	}
}

auto WorkoutEdit::inspect() -> bool {
	ImGui::Text("%s", m_name);
	static constexpr auto flags_v = ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen;
	auto ret = false;
	for (auto& segment : m_segments) {
		if (ImGui::TreeNodeEx(segment.display_name.c_str(), flags_v)) {
			ret |= segment.inspect();
			ImGui::TreePop();
		}
	}
	return ret;
}

auto WorkoutEdit::SegmentEdit::inspect() -> bool {
	auto ret = false;
	for (std::size_t i = 0; i < text_events.size(); ++i) {
		auto& edit = text_events.at(i);
		auto const time = format_time(edit.text_event.timeoffset);
		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 100.0f);
		if (edit.input_text.update(FixedString{"{}##{}", time.view(), i}.c_str())) {
			edit.text_event.set_message(std::string{edit.input_text.as_view()}.c_str());
			ret = true;
		}
		ImGui::Separator();
	}
	return ret;
}
} // namespace wme
