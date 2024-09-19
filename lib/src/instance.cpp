#include "cli/instance.hpp"



namespace cli {
	Instance::Instance() noexcept :
		m_args {}
	{

	}


	Instance::~Instance() noexcept {

	}


	cli::Result Instance::construct(int argc, char **argv) noexcept {
		if (argc != 1)
			return cli::Result::eSuccess;

		m_args.assign(argv + 1, argv + argc);
		return cli::Result::eSuccess;
	}


} // namespace cli