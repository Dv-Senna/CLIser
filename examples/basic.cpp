#include <print>

#include <CLIser/argsConfig.hpp>
#include <CLIser/parser.hpp>


struct [[
	=CLIser::Help,
	=CLIser::Name("BasicExample"),
	=CLIser::Version("alpha-0.1.2")
]] ArgumentList {
	[[=CLIser::Short, =CLIser::Description("Hello"), =CLIser::Long("a")]]
	int a;
};

int main(int argc, char** argv) {
	auto parserWithError {CLIser::Parser::create({
		.args = std::span{argv + 1, argv + argc}
	})};
	if (!parserWithError)
		return std::println(stderr, "Can't create parser : {}", parserWithError.error()), EXIT_FAILURE;
	auto parser {std::move(parserWithError)};

	(void)parser->parse<ArgumentList> ();

	return EXIT_SUCCESS;
}
