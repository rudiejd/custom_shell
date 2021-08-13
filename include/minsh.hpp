/*
 * minsh.cpp
 *
 * Created on: Aug 8, 2021
 * Author: rudiejd <rudiejd@miamioh.edu>
 */
#include <iostream>
#include <string>
#include <vector>

namespace minsh {
class Minsh {
 private:
  std::vector<std::string> history;
  std::istream& is;
  std::ostream& os;
  const std::vector<std::string> splitCommand(std::string str, char sub);
  int forkNexec(std::vector<std::string>& argList);
  void executeCommand(int& cp, std::string line, bool& empty);
  void wait(int childPid);
  bool doCustomCommand(std::vector<std::string>& command);

 public:
  Minsh(std::istream& inIs, std::ostream& inOs) : is(inIs), os(inOs){};
  Minsh() : is(std::cin), os(std::cout){};
  void process(const std::string& prompt = "∫ ", bool parallel = false);
};
}  // namespace minsh
