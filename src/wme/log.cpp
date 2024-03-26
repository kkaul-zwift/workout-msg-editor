#include <wme/log.hpp>
#include <iostream>

#if defined(_WIN32)
#include <Windows.h>
#endif

namespace wme {
void log::print(char const level, std::string_view const message) {
	auto& out = level == 'E' ? std::cerr : std::cout;
	auto const msg = fmt::format("[{}] {}\n", level, message);
	out << msg;
#if defined(_WIN32)
	OutputDebugStringA(msg.c_str());
#endif
}
} // namespace wme
