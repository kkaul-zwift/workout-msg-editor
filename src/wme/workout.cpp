#include <fmt/format.h>
#include <wme/workout.hpp>
#include <charconv>
#include <fstream>

namespace wme {
using namespace std::string_view_literals;
using namespace std::chrono_literals;

using tinyxml2::XMLDocument;
using tinyxml2::XMLElement;

namespace {
[[nodiscard]] auto to_seconds(std::string_view const text) {
	if (text.empty()) { return -1s; }
	auto const* text_end = text.data() + text.size();
	auto ret = int{};
	auto const [ptr, ec] = std::from_chars(text.data(), text_end, ret);
	if (ptr != text_end || ec != std::errc{}) { return -1s; }
	return std::chrono::seconds{ret};
}

[[nodiscard]] auto read_string_from(std::string& out, char const* path) {
	auto file = std::ifstream{path, std::ios::binary | std::ios::ate};
	if (!file) { return false; }
	auto const size = file.tellg();
	file.seekg({}, std::ios::beg);
	out.resize(static_cast<std::size_t>(size));
	file.read(out.data(), size);
	return true;
}

struct Loader {
	Workout result{};

	void load(char const* xml_text) {
		result.document = std::make_shared<XMLDocument>();
		if (result.document->Parse(xml_text) != tinyxml2::XML_SUCCESS) {
			throw std::runtime_error{"Failed to parse workout file as XML"};
		}

		auto* root = result.document->RootElement();
		if (root == nullptr || root->Name() != "workout_file"sv) {
			throw std::runtime_error{"Workout file missing '<workout_file>' as root element"};
		}

		auto const* name = root->FirstChildElement("name");
		if (name == nullptr) { throw std::runtime_error{"Workout file missing '<name>' element"}; }
		result.name = name->GetText();

		auto* workout = root->FirstChildElement("workout");
		if (workout == nullptr) { return; }

		for (auto* node = workout->FirstChild(); node != nullptr; node = node->NextSibling()) {
			auto* in_segment = node->ToElement();
			if (in_segment == nullptr) { continue; }
			load_segment(*in_segment);
		}
	}

	void load_segment(XMLElement& segment) {
		auto out_segment = Segment{
			.element = segment,
			.index = static_cast<int>(result.segments.size()),
			.name = segment.Name(),
		};
		for (auto* node = segment.FirstChildElement("textevent"); node != nullptr;
			 node = node->NextSiblingElement("textevent")) {
			auto* in_event = node->ToElement();
			if (in_event == nullptr) { continue; }
			out_segment.text_events.push_back(TextEvent{.element = *in_event});
		}
		result.segments.push_back(std::move(out_segment));
	}
};
} // namespace

auto TextEvent::get_message() const -> char const* { return element.get().Attribute("message"); }

// NOLINTNEXTLINE(readability-make-member-function-const)
void TextEvent::set_message(char const* message) { element.get().SetAttribute("message", message); }

auto TextEvent::get_timeoffset() const -> std::chrono::seconds {
	return std::chrono::seconds{element.get().IntAttribute("timeoffset")};
}

// NOLINTNEXTLINE(readability-make-member-function-const)
void TextEvent::set_timeoffset(std::chrono::seconds const value) {
	element.get().SetAttribute("timeoffset", value.count());
}

void Workout::load_from_xml(char const* xml_text) noexcept(false) {
	auto loader = Loader{};
	loader.load(xml_text);
	*this = std::move(loader.result);
}

void Workout::load_from_file(char const* file_path) noexcept(false) {
	auto xml_text = std::string{};
	if (!read_string_from(xml_text, file_path)) { throw std::runtime_error{"Failed to read file"}; }
	load_from_xml(xml_text.c_str());
}

auto Workout::save_to_file(char const* file_path) const -> bool {
	if (!document) { return false; }
	return document->SaveFile(file_path) == tinyxml2::XML_SUCCESS;
}
} // namespace wme
