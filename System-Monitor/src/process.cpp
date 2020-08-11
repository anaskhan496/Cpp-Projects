#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"
#include "linux_parser.h"
using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid): pid_(pid){
    cpu_utilization_ = Process::CpuUtilization();
 }
// TODO: Return this process's ID
int& Process::Pid() { return pid_; }

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() { 
    long elapsed_time = LinuxParser::UpTime() - LinuxParser::UpTime(pid_);
    cpu_utilization_ = ((float) LinuxParser::ActiveJiffies(pid_) / sysconf(_SC_CLK_TCK)) / elapsed_time;
    return cpu_utilization_; 
}

// TODO: Return the command that generated this process
string Process::Command() { return LinuxParser::Command(pid_); }

// TODO: Return this process's memory utilization
string Process::Ram() { return LinuxParser::Ram(pid_); }

// TODO: Return the user (name) that generated this process
string Process::User() { return LinuxParser::User(pid_); }

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() { return LinuxParser::UpTime(pid_); }

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a) const { 
    return (this->cpu_utilization_ > a.cpu_utilization_); 
}