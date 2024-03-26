#pragma once
#include <fmt/format.h>
#include <stdexcept>

namespace wme {
struct Error : std::runtime_error {
	template <typename... Args>
	Error(fmt::format_string<Args...> fmt, Args&&... args)
		: std::runtime_error(fmt::format(fmt, std::forward<Args>(args)...)) {}
};
} // namespace wme
