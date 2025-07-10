#include <print>

#include <CLIser/parser.hpp>


int main(int argc, char** argv) {
	auto parserWithError {CLIser::Parser::create({
		.args = std::span{argv + 1, argv + argc}
	})};
	if (!parserWithError)
		return std::println(stderr, "Can't create parser : {}", parserWithError.error()), EXIT_FAILURE;

	return EXIT_SUCCESS;
}
