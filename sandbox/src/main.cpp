#include <cli/instance.hpp>
#include <cli/argumentParser.hpp>



int main(int argc, char **argv) {
	cli::Instance instance {};

	if (instance.construct(argc, argv) != cli::Result::eSuccess)
		return EXIT_FAILURE;

	return EXIT_SUCCESS;
}
