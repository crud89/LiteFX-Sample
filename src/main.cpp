#include "main.h"

#include "sample.h"

// CLI11 parses optional values as double by default, which yields an implicit-cast warning.
#pragma warning(disable: 4244)

#include <CLI/CLI.hpp>
#include <iostream>
#include <filesystem>
#include <print>

int main(const int argc, const char** argv)
{
#ifdef WIN32
	// Enable console colors.
	HANDLE console = ::GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD consoleMode = 0;

	if (console == INVALID_HANDLE_VALUE || !::GetConsoleMode(console, &consoleMode))
		return static_cast<int>(::GetLastError());

	::SetConsoleMode(console, consoleMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
#endif

	// Parse the command line parameters.
	const String appName{ SampleApp::Name() };

	CLI::App commandLine{ "Demonstrates basic drawing techniques.", appName };

	Optional<UInt32> adapterId;
	commandLine.add_option("-a,--adapter", adapterId)->take_first();
	auto validationLayers = commandLine.add_option("-l,--vk-validation-layers")->take_all();

	try
	{
		commandLine.parse(argc, argv);
	}
	catch (const CLI::ParseError& ex)
	{
		return commandLine.exit(ex);
	}

	// Turn the validation layers into a list.
	Array<String> enabledLayers;

	if (validationLayers->count() > 0)
		for (const auto& result : validationLayers->results())
			enabledLayers.push_back(result);

	// Create glfw window.
	if (!::glfwInit())
		throw std::runtime_error("Unable to initialize glfw.");

	::glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	::glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

	auto window = GlfwWindowPtr(::glfwCreateWindow(800, 600, appName.c_str(), nullptr, nullptr));

	// Get the required Vulkan extensions from glfw.
	uint32_t extensions = 0;
	const char** extensionNames = ::glfwGetRequiredInstanceExtensions(&extensions);
	Array<String> requiredExtensions;

	for (uint32_t i(0); i < extensions; ++i)
		requiredExtensions.emplace_back(extensionNames[i]);

	// Create the app.
	try
	{
		UniquePtr<App> app = App::build<SampleApp>(std::move(window), adapterId)
			.logTo<ConsoleSink>(LogLevel::Trace)
			.logTo<RollingFileSink>("sample.log", LogLevel::Debug)
#ifdef LITEFX_BUILD_VULKAN_BACKEND
			.useBackend<VulkanBackend>(requiredExtensions, enabledLayers)
#endif // LITEFX_BUILD_VULKAN_BACKEND
#ifdef LITEFX_BUILD_DIRECTX_12_BACKEND
			.useBackend<DirectX12Backend>()
#endif // LITEFX_BUILD_DIRECTX_12_BACKEND
			;

		app->run();
	}
	catch (const LiteFX::Exception& ex)
	{
		std::cerr << "\033[3;41;37mUnhandled exception: " << ex.what() << '\n' << "at: " << ex.trace() << "\033[0m\n";

		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}