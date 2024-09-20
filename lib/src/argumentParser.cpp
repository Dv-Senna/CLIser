#include "cli/argumentParser.hpp"



namespace cli {
	ArgumentParser::ArgumentParser() :
		m_flags {},
		m_args {}
	{

	}


	cli::Result ArgumentParser::construct(const cli::ArgumentParser::Infos &infos) {
		return cli::Result::eSuccess;
	}


} // namespace cli
