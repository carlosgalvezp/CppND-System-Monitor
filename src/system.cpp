#include <unistd.h>
#include <cstddef>
#include <set>
#include <string>
#include <vector>

#include "linux_parser.h"
#include "process.h"
#include "processor.h"
#include "system.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;

System::System() :
  cpu_{},
  kernel_{LinuxParser::Kernel()},
  operating_system_{LinuxParser::OperatingSystem()}
{}

Processor& System::Cpu() { return cpu_; }

vector<Process> System::Processes() {
  std::vector<Process> processes;

  std::vector<int> pids = LinuxParser::Pids();
  for (int pid : pids)
  {
    std::string user = LinuxParser::User(pid);
    std::string command = LinuxParser::Command(pid);
    float cpu_utilization = LinuxParser::CpuUtilization(pid);
    std::string ram = LinuxParser::Ram(pid);
    long int uptime = LinuxParser::UpTime(pid);
    processes.emplace_back(pid, user, command, cpu_utilization, ram, uptime);
  }

  std::sort(processes.begin(), processes.end());
  return processes;
}

std::string System::Kernel() { return kernel_; }

float System::MemoryUtilization() { return LinuxParser::MemoryUtilization(); }

std::string System::OperatingSystem() {  return operating_system_; }

int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }

int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }

long int System::UpTime() { return LinuxParser::UpTime(); }
