#include <wme/error.hpp>
#include <wme/fixed_string.hpp>
#include <wme/log.hpp>
#include <wme/util.hpp>
#include <wme/workout_edit.hpp>
#include <span>

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

class WorkoutEdit::TextEventEdit::Inspector {
  public:
	explicit Inspector(Options const& options, std::span<TextEventEdit> events, std::size_t const index)
		: m_options(options), m_edit(events[index]), m_index(index), m_timeoffset(m_edit.text_event.get_timeoffset()) {}

	auto message(float const width) const -> bool {
		auto const time = format_time(m_timeoffset);
		ImGui::SetNextItemWidth(width);
		if (m_edit.input_text.update(FixedString{"{}##message_{}", time.view(), m_index}.c_str())) {
			m_edit.text_event.set_message(std::string{m_edit.input_text.as_view()}.c_str());
			return true;
		}
		return false;
	}

	auto timeoffset(float const width) const -> bool {
		auto seconds = static_cast<int>(m_timeoffset.count());
		ImGui::SetNextItemWidth(width);
		auto ret = false;
		if (ImGui::DragInt(FixedString{"s##seconds_{}", m_index}.c_str(), &seconds, 1.0f, 0, 3600)) {
			m_edit.text_event.set_timeoffset(std::chrono::seconds{seconds});
			ret = true;
		}
		if (m_options.show_tooltips) { ImGui::SetItemTooltip("Click and drag to edit, double click to enter value"); }
		return ret;
	}

	auto should_remove(float const width) const -> bool {
		ImGui::SetNextItemWidth(width);
		auto const ret = red_button(FixedString{"x##delete_{}", m_index}.c_str());
		if (m_options.show_tooltips) { ImGui::SetItemTooltip("Click to remove this textevent"); }
		return ret;
	}

	[[nodiscard]] auto get_element() const -> tinyxml2::XMLElement& { return m_edit.text_event.element; }

  private:
	Options const& m_options;
	TextEventEdit& m_edit;
	std::size_t m_index;
	std::chrono::seconds m_timeoffset;
};

WorkoutEdit::WorkoutEdit(Workout const& workout) : m_document(workout.document) {
	m_segments.reserve(workout.segments.size());
	for (auto const& in_segment : workout.segments) {
		auto out_segment = SegmentEdit{.element = in_segment.element};
		out_segment.display_name = fmt::format("{}. {}", in_segment.index, in_segment.name);
		for (auto const& in_event : in_segment.text_events) { out_segment.text_events.emplace_back(in_event); }
		m_segments.push_back(std::move(out_segment));
	}
}

auto WorkoutEdit::inspect(Options const& options) -> bool {
	ImGui::Text("%s", m_name);
	static constexpr auto flags_v = ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen;
	auto ret = false;
	for (auto& segment : m_segments) {
		if (ImGui::TreeNodeEx(segment.display_name.c_str(), flags_v)) {
			ret |= segment.inspect(options, *m_document);
			ImGui::TreePop();
		}
	}
	return ret;
}

auto WorkoutEdit::SegmentEdit::inspect(Options const& options, tinyxml2::XMLDocument& document) -> bool {
	auto ret = false;
	for (std::size_t i = 0; i < text_events.size(); ++i) {
		auto const inspector = TextEventEdit::Inspector{options, text_events, i};

		ret |= inspector.message(ImGui::GetContentRegionAvail().x - 250.0f);

		ImGui::SameLine();
		ret |= inspector.timeoffset(50.0f);

		ImGui::SameLine();
		if (inspector.should_remove(50.0f)) {
			document.DeleteNode(&inspector.get_element());
			text_events.erase(text_events.begin() + i);
			ret = true;
			break; // break out of loop since container has been modified
		}

		ImGui::Separator();
	}

	if (ImGui::Button("Add")) { ret |= push_textevent(); }
	if (options.show_tooltips) { ImGui::SetItemTooltip("Click to add a new textevent"); }

	return ret;
}

auto WorkoutEdit::SegmentEdit::push_textevent() -> bool {
	auto* new_element = element.get().InsertNewChildElement("textevent");
	if (new_element == nullptr) {
		log::error("Failed to create new XML element");
		return false;
	}

	auto const next_timeoffset = [&] {
		if (text_events.empty()) { return 0s; }
		return text_events.back().text_event.get_timeoffset() + 10s;
	}();
	new_element->SetAttribute("timeoffset", next_timeoffset.count());
	new_element->SetAttribute("message", "Message");

	auto new_text_event = TextEvent{.element = *new_element};
	text_events.emplace_back(new_text_event);

	return true;
}
} // namespace wme
