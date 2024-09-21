#include "cli/argumentParser.hpp"



namespace cli {
	ArgumentParser::ArgumentParser() :
		m_flags {},
		m_args {}
	{

	}


	cli::Result ArgumentParser::construct(const cli::ArgumentParser::Infos &infos) {
		for (const auto &arg : infos.args) {
			auto flag {infos.expectedFlags.find(arg)};
			if (flag != infos.expectedFlags.end()) {
				if (flag->second & cli::FlagValueTypeBits::eBoolean) {
					m_flags[flag->first].type = cli::FlagValueTypeBits::eBoolean;
					m_flags[flag->first].boolean = true;
				}

				continue;
			}


		}

		return cli::Result::eSuccess;
	}


} // namespace cli
