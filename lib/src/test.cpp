#include "test.hpp"

#include <iostream>


namespace test {
	void sayHello() {
		#ifdef CLI_WINDOWS
			std::cout << "Hello from Windows CLIser !" << std::endl;
		#elifdef CLI_LINUX
			std::cout << "Hello from Linux CLIser !" << std::endl;
		#elifdef CLI_APPLE
			std::cout << "Hello from Apple CLIser !" << std::endl;
		#endif
	}
}