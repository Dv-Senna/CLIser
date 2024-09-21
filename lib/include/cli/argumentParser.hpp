#pragma once

#include <map>
#include <string>
#include <vector>

#include "cli/result.hpp"
#include "cli/utils/bitfield.hpp"



namespace cli {
	CLI_CREATE_BITFIELD(FlagValueType,
		eString      = 0x01,
		eInteger     = 0x02,
		eFloat       = 0x04,
		eBoolean     = 0x08,
		eMultistring = 0x10
	);


	struct FlagValue {
		cli::FlagValueTypeBits type;
		union {
			std::string string;
			std::vector<std::string> multistring;
			std::int64_t integer;
			float floating;
			bool boolean;
		};
	};


	class ArgumentParser {
		public:
			struct Infos {
				std::map<std::string, cli::FlagValueType> expectedFlags;
				std::vector<std::string> args;
			};

			ArgumentParser();
			~ArgumentParser() = default;

			cli::Result construct(const cli::ArgumentParser::Infos &infos);

			inline const std::map<std::string, cli::FlagValue> &getFlags() const noexcept {return m_flags;}
			inline const std::vector<std::string> &getArgs() const noexcept {return m_args;}

		private:
			std::map<std::string, cli::FlagValue> m_flags;
			std::vector<std::string> m_args;
	};

} // namespace cli
