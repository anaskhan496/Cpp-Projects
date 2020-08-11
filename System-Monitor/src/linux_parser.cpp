#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <iostream>
#include "linux_parser.h"
#include "processor.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath); //reads from /etc/os-release file. 
  //the file consists of jey value pairs alongwith white spaces which will have to be taken care of. 
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) { // as long as we are able to read a line from the file, keep going
      std::replace(line.begin(), line.end(), ' ', '_'); // replace whitespaces with characters for easier parsing
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) { //pop tokens to key and value
        if (key == "PRETTY_NAME") { // if true it will return Ubuntu 16.04.5 LTS
          std::replace(value.begin(), value.end(), '_', ' '); //swap back the characters with whitespace 
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename); // creates an input file stream from operating system kernel version (proc directory + version).
  // type cat /proc/version to see the output on terminal. From the terminal we need to obtain the version which is the third token.
  // Ifstream: File handling class that signifies the input file stream and is used for reading data from the file.
  if (stream.is_open()) { //checks if we are able top open the input filestream (/proc/version)
    std::getline(stream, line);
    std::istringstream linestream(line); //linestream variable reads the line string from proc version file using istringstream
    linestream >> os >> version >> kernel; // write the contents of linestream to the strings os and kernel. Kernel will return the version name. 
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// TODO: Read and return the system memory utilization
// Open the /proc/meminfo filestream and calculate the memory utilization
float LinuxParser::MemoryUtilization() { 
  float utilization;
  string line, temp, value, unit;
  vector<float> meminfo_data {};
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.is_open()){
    for(int i=0; i<4; i++){
      std::getline(stream, line);
      std::istringstream linestream(line);
      linestream >> temp >> value >> unit;
      float val = stof(value); // convert string to float
      meminfo_data.push_back(val);
    }
  }
  utilization = (meminfo_data.at(0) - meminfo_data.at(1) ) / meminfo_data.at(0);
  // utilization = memtotal - memfree  / memtotal   [meminfo_data.at(index value)]
  return utilization;
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() { 
  string line, value1, value2;
  long uptime;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if(stream.is_open()){
    std::getline(stream,line);
    std::istringstream linestream(line);
    linestream >> value1 >> value2;
    float temp = stof(value1); //converts the uptime from string to float
    uptime = static_cast<long>(temp);
  }
  return uptime;
}

/*
A jiffy is a kernel unit of time declared in <linux/jiffies.h>. 
To understand jiffies, we need to introduce a new constant, HZ, which is
the number of times jiffies is incremented in one second. Each increment is called a tick. 
In other words, HZ represents the size of a jiffy. HZ depends on the hardware and on the kernel 
version, and also determines how frequently the clock interrupt fires.
This is configurable on some architectures, fixed on other ones. What it means is that 
jiffies is incremented HZ times every second. If HZ = 1,000, then it is incremented 
1,000 times (that is, one tick every 1/1,000 seconds). Once defined, the programmable 
interrupt timer (PIT), which is a hardware component, is programmed with that value ...
https://www.oreilly.com/library/view/linux-device-drivers/9781785280009/4041820a-bbe4-4502-8ef9-d1913e133332.xhtml#:~:text=A%20jiffy%20is%20a%20kernel,declared%20in%20%3Clinux%2Fjiffies.&text=What%20it%20means%20is%20that,every%201%2F1%2C000%20seconds).
*/

/*
Reference links for calculation of jiffies, activejiffies(pid), activejiffies, idlejiffies, and cpuutil
https://web.archive.org/web/20130302063336/http://www.lindevdoc.org/wiki//proc/pid/stat
https://stackoverflow.com/questions/16726779/how-do-i-get-the-total-cpu-usage-of-an-application-from-proc-pid-stat
*/

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { 
  string line, key, value;
  long jiffies = 0;
  vector<long> all_values;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if(filestream.is_open()){
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> key;
    if (key == "cpu"){
      while(linestream >> value){ 
        float v = std::stof(value);
        all_values.push_back(v);
      }
    }
  }
  jiffies = all_values.at(CPUStates::kUser_) + all_values.at(CPUStates::kIdle_) + all_values.at(CPUStates::kIOwait_) + \
            all_values.at(CPUStates::kNice_) + all_values.at(kSystem_) + all_values.at(CPUStates::kIRQ_) + \
            all_values.at(CPUStates::kSoftIRQ_) + all_values.at(CPUStates::kSteal_); 
            // User_, Nice_, System_, Idle_, IWwait_, IRQ_, SoftIRQ_, Steal_
  return jiffies;
}

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) { 
  string line, ignore;
  long active_jiffies = 0;
  long total_jiffies = 0;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if(filestream.is_open()){
    std::getline(filestream, line);
    std::istringstream linestream(line);
    for(int id = 0; id < 17; id++){
      if (id == Proc_Pid_Stat::kutime_ || id == Proc_Pid_Stat::kstime_ || id == Proc_Pid_Stat::kcutime_ || id == Proc_Pid_Stat::kcstime_){
        linestream >> active_jiffies;
        total_jiffies += active_jiffies;
      }
      else{
        linestream >> ignore;
      }
    }
  }
  return total_jiffies; 
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { 
  string line, key, value;
  vector<long> all_values;
  long idle_jiffies = 0;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()){
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> key;
    if (key == "cpu"){
      while(linestream >> value){ 
        float v = std::stof(value);
        all_values.push_back(v);
      }
    }
  }
  idle_jiffies = all_values.at(CPUStates::kIdle_) + all_values.at(CPUStates::kIOwait_);
  std::cout<<idle_jiffies;
  return idle_jiffies;
}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { 
  return LinuxParser::Jiffies() - LinuxParser::IdleJiffies(); 
}

// TODO: Read and return CPU utilization
// vector<string> LinuxParser::CpuUtilization() { return {}; }

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() { 
  string line, value, key;
  int total_processes = 0;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()){
    while (std::getline(filestream, line)){
      std::istringstream linestream(line);
      while(linestream >> key){
        if (key == "processes"){
          linestream >> value;
          total_processes = std::stoi(value);
          return total_processes;
        }
      }
    }
  }
  return total_processes;
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() { 
  string line, value, key;
  int running_processes = 0;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()){
    while (std::getline(filestream, line)){
      std::istringstream linestream(line);
      while(linestream >> key){
        if (key == "procs_running"){
          linestream >> value;
          running_processes = std::stoi(value);
          return running_processes;
          }
        }
      }
    }   
  return running_processes; 
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) { 
  string cmdline;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
  if (filestream.is_open()){
    std::getline(filestream, cmdline);
    // std::istringstream linestream(line);
    // linestream >> cmdline;
  }
  return cmdline; 
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) { 
  string line, key, value;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (filestream.is_open()){
    while(std::getline(filestream, line)){
      std::replace(line.begin(),line.end(),':',' ');
      std::istringstream linestream(line);
      while(linestream >> key){
        if(key == "VmSize"){
          linestream >> value;
          int v = std::stoi(value);
          return std::to_string(v/1000);
        }
      }
    }
  }
  return string(); 
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) { 
  string line, key, value;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (filestream.is_open()){
    while(std::getline(filestream, line)){
      std::replace(line.begin(),line.end(),':',' ');
      std::istringstream linestream(line);
      while(linestream >> key){
        if(key == "Uid"){
          linestream >> value;
          return value;
        }
      }
    }
  }
  return string(); 
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) { 
  string uid = LinuxParser::Uid(pid);
  string line, key, value;
  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()){
    while (std::getline(filestream,line)){
      std::replace(line.begin(),line.end(), 'x',' ');
      std::replace(line.begin(), line.end(), ':',' ');
      std::istringstream linestream(line);
      while(linestream >> key >> value){
        if (value == uid){
          return key;
        }
      }
    }
  }
  return string(); 
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
// time when the process started, measured in nanoseconds since the system boot
long LinuxParser::UpTime(int pid) { 
  string line, ignore, value;
  long uptime;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (filestream.is_open()){
    std::getline(filestream, line);
    std::istringstream linestream(line);
    for (int i=0; i<22; i++){
      linestream >> value;
      if (i == Proc_Pid_Stat::kstarttime_){
        uptime = stof(value) / sysconf(_SC_CLK_TCK);
        return uptime;
      }
    }
  }
  return 0;
}