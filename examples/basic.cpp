#include <print>

#include <CLIser/argsConfig.hpp>
#include <CLIser/parser.hpp>


struct [[
	=CLIser::Help,
	=CLIser::Name("BasicExample"),
	=CLIser::Version("alpha-0.1.2")
	=CLIser::VersionDescription("Special development version")
]] ArgumentList {
	[[=CLIser::Description("Say hello to the user"), =CLIser::Long("hello")]]
	bool sayHello;
	[[=CLIser::Short, =CLIser::Description("Specify the output file")]]
	std::string_view output;
	[[=CLIser::Short("k"), =CLIser::Long("kount")]]
	int count;
};

int main(int argc, char** argv) {
	auto parserWithError {CLIser::Parser::create({
		.args = std::span{argv, argv + argc}
	})};
	if (!parserWithError)
		return std::println(stderr, "Can't create parser : {}", parserWithError.error()), EXIT_FAILURE;
	auto parser {std::move(parserWithError)};

	(void)parser->parse<ArgumentList> ();

/*	std::string text {"Hello world, I'm will test really small line size, like 5 characters"};
	auto lines {text | CLIser::utils::views::chunk(5)};
	for (auto it {lines.begin()}; it != lines.end(); ++it) {
		std::println("line : {}", *it);
	}*/

	return EXIT_SUCCESS;
}
