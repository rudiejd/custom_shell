#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <minsh.hpp>
#include <sstream>
#include <string>
#include <unistd.h>
#include <pwd.h>

using namespace minsh;

TEST_CASE( "Test changing directories", "[main]" ) {
	std::stringstream sin, sout;
	Minsh m(sin, sout);
	sin << "cd ~/" << std::endl << "exit";
	m.process();
	std::string output =  sout.str(); 
	struct passwd *pw = getpwuid(getuid());
	const char* home = pw->pw_dir;
	REQUIRE(output.find(home) != std::string::npos);
}
