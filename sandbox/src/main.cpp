#include <print>

#include <CLIser/context.hpp>


int main(int argc, char **argv) {
	CLIser::Context::CreateInfos contextCreateInfos {};
	contextCreateInfos.args = std::span(argv, argc);
	auto context {CLIser::Context::construct(contextCreateInfos)};

	std::println("args : {}", context.getArgs());

	return EXIT_SUCCESS;
}
