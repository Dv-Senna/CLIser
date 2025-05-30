#include <print>

#include <CLIser/context.hpp>
#include <CLIser/argument.hpp>


int main(int argc, char **argv) {
	CLIser::Context::CreateInfos contextCreateInfos {};
	contextCreateInfos.args = std::span(argv, argc);
	auto context {CLIser::Context::construct(contextCreateInfos)};

	auto name {CLIser::Argument<std::string>::construct({
		.context = context,
		.tags = {"n", "name"},
		.description = "The name of the user"
	})};

	auto age {CLIser::Argument<int>::construct({
		.context = context,
		.tags = {"age"},
		.description = "Age of the user",
		.defaultValue = 18,
		.condition = [](int age) {return age >= 18 && age <= 100;}
	})};

	if (!context.parse())
		return std::println(stderr, "Can't parse args"), EXIT_FAILURE;

	std::println("args : {}", context.getArgs());
	if (name)
		std::println("Name : {}", *name);
	else
		std::println("No name");

	std::println("Age : {}", *age);

	return EXIT_SUCCESS;
}
