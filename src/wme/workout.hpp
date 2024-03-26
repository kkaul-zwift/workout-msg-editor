#pragma once
#include <tinyxml2.h>
#include <chrono>
#include <functional>
#include <memory>
#include <vector>

namespace wme {
struct TextEvent {
	std::chrono::seconds timeoffset;
	std::reference_wrapper<tinyxml2::XMLElement> element;

	[[nodiscard]] auto get_message() const -> char const*;
	void set_message(char const* message);
};

struct Segment {
	int index{};
	char const* name{""};
	std::vector<TextEvent> text_events{};
};

struct Workout {
	std::shared_ptr<tinyxml2::XMLDocument> document{};
	char const* name{""};
	std::vector<Segment> segments{};

	void load_from_file(char const* file_path) noexcept(false);
	void load_from_xml(char const* xml_text) noexcept(false);

	[[nodiscard]] auto save_to_file(char const* file_path) const -> bool;
};
} // namespace wme
