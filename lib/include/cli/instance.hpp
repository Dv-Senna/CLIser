#pragma once

#include <string>
#include <vector>

#include "cli/core.hpp"
#include "cli/result.hpp"



namespace cli {
	class CLI_CORE Instance final {
		public:
			Instance() noexcept;
			~Instance() noexcept;

			cli::Result construct(int argc, char **argv) noexcept;

			inline const std::vector<std::string> &getArgs() const noexcept {return m_args;}

		private:
			std::vector<std::string> m_args;
	};

} // namespace cli