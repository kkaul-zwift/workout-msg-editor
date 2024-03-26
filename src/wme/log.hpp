#pragma once
#include <fmt/format.h>
#include <iostream>

namespace wme::log {
void print(char level, std::string_view message);

template <typename... Args>
void error(fmt::format_string<Args...> fmt, Args&&... args) {
	print('E', fmt::format(fmt, std::forward<Args>(args)...));
}

template <typename... Args>
void warn(fmt::format_string<Args...> fmt, Args&&... args) {
	print('W', fmt::format(fmt, std::forward<Args>(args)...));
}

template <typename... Args>
void info(fmt::format_string<Args...> fmt, Args&&... args) {
	print('I', fmt::format(fmt, std::forward<Args>(args)...));
}
} // namespace wme::log
