#include "processor.h"
#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>

#include "linux_parser.h"


using std::stof;
using std::string;
using std::to_string;
using std::vector;
using std::stoi;
using std::stol;
using namespace LinuxParser;

// TODO: Return the aggregate CPU utilization
// returns the cpu consumption of the all the processes combined in percentage. This method is used 
// by the method CPU in system class
float Processor::Utilization() { 
    cpu_cons = ((float) ActiveJiffies() / Jiffies());
    return cpu_cons; 
}