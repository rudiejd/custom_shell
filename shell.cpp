/*
 * shell.h
 *
 * Created on: Jul 19, 2021
 * Author: rudiejd <rudiejd@miamioh.edu>
 */

#include <boost/asio.hpp>
#include <boost/format.hpp>
#include <cstdlib>
#include <cstring>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <set>
#include <tuple>
#include <iomanip>
#include <sys/wait.h>


namespace custom_shell {
    using iss = std::stringstream;

    /**
    * @brief Helper method to split commands on a certain character
    * @param str The command std::string to be split
    * @param spl The character to split on
    * @return std::vector<std::string> (Vector of std::strings split on spl)
    */
    const std::vector<std::string> splitCommand(std::string str, char sub = ' ') {
        std::vector<std::string> res;
        std::string word;
        // replace all of the split characters with space to use std::string stream
        replace(std::begin(str), std::end(str), sub, ' ');
        // create std::string stream on modified str
        iss ss(str);
        // push each "word" delimited by spl into result std::vector
        while (ss >> quoted(word)) {
            res.push_back(word);
        }
        return res;
    }

    /**
    * Fork and execute a command. Returns process id (PID)
    */
    int forkNexec(std::vector<std::string>& argList) {
        // Fork and save the pid of the child process
        int childPid = fork();
        // Call the myExec helper method in the child
        if (childPid == 0) {
            // We are in the child process
            std::vector<char*> args;
            for (auto s : argList) {
                std::string s2 = s;
                char* writeCpy = new char[s.size() + 1];
                copy(s.begin(), s.end(), writeCpy);
                writeCpy[s.size()] = '\0';
                args.push_back(writeCpy);
            }
            args.push_back(nullptr);
            args.shrink_to_fit();
            execvp(args[0], const_cast<char* const *>(args.data()));
            throw std::runtime_error("Call to execvp failed for " + argList[0]);
        }
        // Control drops here only in the parent process!
        return childPid;
    }

    /**
    * Executes a shell command, ignoring comments and empty lines.
    * @param line Line of shell code to execute
    * @param bool Reference to boolean that will be set true if process is empty
    */
    void executeCommand(int& cp, std::string line, bool& empty) {
        empty = false;
        std::vector<std::string> command;

        // If the line is empty or starts with a #, just continue
        if (line == "" || line[0] == '#') {
            empty = true;
            return;
        }
        // Split command on space using helper method
        command = splitCommand(line);

        // Fork and execute command on new child process
        cp = forkNexec(command);
    }

    void wait(int childPid) {
        int exitCode = 0;
        waitpid(childPid, &exitCode, 0);
		// alert for irregular exit code
		if (exitCode != 0) {
			std::cout << "Error, exit code: " << exitCode << std::endl;
		}
    }

    /**
    * @brief Main method for executing shell commands. Reads commands from generic
    * stream and prints output to std::cout
    * @param is Stream from which to read commands
    * @param prompt Prompt sent to user (default " > ")
    * @param parallel Whether the commands should be executed in parallel
    */
    void process(std::istream& is = std::cin,
            const std::string& prompt = "∫ ", bool parallel = false) {
        std::string line;
        // Vector containing all the child processes spawned (only necessary for
        // parallel execution
        std::vector<int> childProcs;
        // Keep prompting the user and extracting output
		char* login = getlogin();
		char hostname[1024];
		hostname[1023] = '\0';
		gethostname(hostname, 1023);
        while (std::cout << login << "@" << hostname << prompt, getline(is, line)) {
            // If the line asks us to exit, simply return to main.
            if (line == "exit") {
                return;
            }
            bool empty;
            int cp;
            executeCommand(cp, line, empty);
            if (!empty) {
                // If we are running serial, wait on the child we spawned
                if (!parallel) {
                    wait(cp);
                } else {
                    childProcs.push_back(cp);
                }
            }
        }
        // If we are running parallel, wait on all spawned processes
        if (parallel) {
            for (int cp : childProcs) {
                wait(cp);
            }
        }
    }
}

int main(int argc, char** argv) {
    custom_shell::process();
    return 0;
}
