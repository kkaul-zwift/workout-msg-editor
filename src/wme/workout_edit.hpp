#pragma once
#include <wme/im_input_text.hpp>
#include <wme/workout.hpp>

namespace wme {
class WorkoutEdit {
  public:
	explicit WorkoutEdit(Workout const& workout);

	auto inspect() -> bool;

  private:
	struct TextEventEdit {
		TextEvent text_event;
		ImInputText input_text{};

		explicit TextEventEdit(TextEvent text_event) : text_event(text_event) {
			input_text.set_text(text_event.get_message());
		}
	};

	struct SegmentEdit {
		std::string display_name{};
		std::chrono::seconds duration{};
		std::vector<TextEventEdit> text_events{};

		auto inspect() -> bool;
	};

	std::shared_ptr<tinyxml2::XMLDocument> m_document{};
	char const* m_name{""};
	std::vector<SegmentEdit> m_segments{};
};
} // namespace wme
