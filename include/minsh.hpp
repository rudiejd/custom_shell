/*
 * minsh.cpp
 *
 * Created on: Aug 8, 2021
 * Author: rudiejd <rudiejd@miamioh.edu>
 */
#include <vector>
#include <string>
#include <iostream>


namespace minsh {
	class Minsh {
		private:
			std::vector<std::string> history;
			const std::vector<std::string> splitCommand(std::string str, char sub);
			int forkNexec(std::vector<std::string>& argList);
			void executeCommand(int& cp, std::string line, bool& empty);
			void wait(int childPid);
		public:
			Minsh() { };	
			void process(std::istream& is = std::cin, const std::string& prompt= "∫ ", bool parallel = false);
			
	};
}
