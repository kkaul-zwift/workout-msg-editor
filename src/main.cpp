#include <fmt/format.h>
#include <wme/app.hpp>
#include <wme/build_version.hpp>
#include <wme/context.hpp>

auto main() -> int {
	using namespace wme;
	auto const title = fmt::format("Workout Message Editor [v{}]", build_version_v);
	auto window = Context::create_window({1280, 720}, title.c_str());
	auto app = App{*window};
	return app.run(Context{std::move(window)});
}

#if defined(_WIN32)
auto WinMain() -> int { return main(); }
#endif
