/*
 * shell.h
 *
 *  Created on: Jul 19, 2021
 *      Author: jd
 */

#include <boost/asio.hpp>
#include <boost/format.hpp>
#include <cstdlib>
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


#ifndef SHELL_H_
#define SHELL_H_

using namespace std;
using iss = istringstream;

/*
 Example user input:

 * > # Comment
 * > echo hello, world!
 *
 * Output:
 * Running: echo hello, world!
 * hello, world!
 * Exit code: 0
 *


 */


// Declaring this skeleton for use in processRemote method
void process(istream& is,
        const string& prompt, bool parallel);


/**
 * @brief Helper method to split commands on a certain character
 * @param str The command string to be split
 * @param spl The character to split on
 * @return vector<string> (Vector of strings split on spl)
 */
const vector<string> splitCommand(string str, char sub = ' ') {
    vector<string> res;
    string word;
    // replace all of the split characters with space to use string stream
    replace(std::begin(str), std::end(str), sub, ' ');
    // create string stream on modified str
    istringstream ss(str);
    // push each "word" delimited by spl into result vector
    while (ss >> quoted(word)) {
        res.push_back(word);
    }
    return res;
}

/**
 * Helper method to break down a URL into hostname, port and path. For
 * example, given the url: "https://localhost:8080/~raodm/one.txt"
 * this method returns <"localhost", "8080", "/~raodm/one.txt">
 *
 * Similarly, given the url: "ftp://ftp.files.miamioh.edu/index.html"
 * this method returns <"ftp.files.miamioh.edu", "80", "/index.html">
 *
 * @param url A string containing a valid URL. The port number in URL
 * is always optional.  The default port number is assumed to be 80.
 *
 * @return This method returns a std::tuple with 3 strings. The 3
 * strings are in the order: hostname, port, and path.  Here we use
 * std::tuple because a method can return only 1 value.  The
 * std::tuple is a convenient class to encapsulate multiple return
 * values into a single return value.
 */
std::tuple<std::string, std::string, std::string>
breakDownURL(const string url) {
    // The values to be returned.
    std::string hostName, port = "80", path = "/";
    // Extract the substrings from the given url into the above
    // variables.  This is very simple 174-level logic problem.

    // Find important positions in URL
    std::string::size_type dblSlashPos = url.find("//");
    std::string::size_type frstSnglSlashPos = url.find("/", dblSlashPos+2);
    std::string::size_type frstColonPos = url.find(":");
    std::string::size_type scndColonPos = url.find(":", frstColonPos+1);

    // Get return values
    if (scndColonPos != std::string::npos) {
       port = url.substr(scndColonPos+1, url.find("/",
                scndColonPos)-scndColonPos-1);
       hostName = url.substr(dblSlashPos+2, scndColonPos-dblSlashPos-1);
    } else {
        hostName = url.substr(dblSlashPos+2, frstSnglSlashPos-(dblSlashPos+2));
    }

    path = url.substr(frstSnglSlashPos);
    // Return 3-values encapsulated into 1-tuple.
    return {hostName, port, path};
}


/**
 * Fetch remote script from web page and feed it to main processing method.
 * @param url URL of script to execute
 * @param par Whether the script should be executed in parallel
 */
void processRemote(string url, bool par) {
    // Extract download URL components.
    std::string hostname, port, path;
    std::tie(hostname, port, path) = breakDownURL(url);

    // Start the download of the file (that the user wants to be
    // processed) at the specified URL.  We use a BOOST tcp::iostream.
    boost::asio::ip::tcp::iostream data(hostname, port);
    data << "GET "   << path     << " HTTP/1.1\r\n"
         << "Host: " << hostname << "\r\n"
         << "Connection: Close\r\n\r\n";
    // Have the helper method process the file's data and print/send
    // results (in HTTP/HTML format) to a given output stream.
    for (std::string hdr; std::getline(data, hdr) && !hdr.empty()
            && hdr != "\r"; ) {
    }

    process(data, "", par);
}
int forkNexec(vector<string>& argList) {
    // Fork and save the pid of the child process
    int childPid = fork();
    // Call the myExec helper method in the child
    if (childPid == 0) {
        // We are in the child process
    	vector<char*> args;
    	for (auto& s : argList) {
    		args.push_back(s.data());
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
void executeCommand(int cp, string line, bool& empty) {
    empty = false;
    vector<string> command;

    // If the line is empty or starts with a #, just continue
    if (line == "" || line[0] == '#') {
        empty = true;
        return;
    }
    // Split command on space using helper method
    command = splitCommand(line);



    if (command[0] == "PARALLEL" || command[0] == "SERIAL") {
        empty = true;
        processRemote(command[1], command[0] == "PARALLEL");
        return;
    }
    cout << "Running:";
    for (auto arg : command) cout << " " << arg;
    cout << endl;
    // Fork and execute command on new child process
    forkNexec(command);
}




void wait(int childPid) {
	int exitCode = 0;
	waitpid(childPid, &exitCode, 0);
	cout << "Exit code: " << exitCode << endl;
}

/**
 * @brief Main method for executing shell commands. Reads commands from generic
 * stream and prints output to cout
 * @param is Stream from which to read commands
 * @param prompt Prompt sent to user (default " > ")
 * @param parallel Whether the commands should be executed in parallel
 */
void process(istream& is = cin,
        const string& prompt = "> ", bool parallel = false) {
    string line;
    // Vector containing all the child processes spawned (only necessary for
    // parallel execution
    vector<int> childProcs;
    // Keep prompting the user and extracting output
    while (cout << prompt, getline(is, line)) {
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



/*
 *
 */
int main(int argc, char** argv) {
    process();
    return 0;
}

#endif /* SHELL_H_ */
