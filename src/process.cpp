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

Process::Process(int pid,
                 std::string const& user,
                 std::string const& command,
                 float cpu_utilization,
                 std::string const& ram,
                 long int uptime) :
    pid_(pid),
    user_(user),
    command_(command),
    cpu_utilization_(cpu_utilization),
    ram_(ram),
    uptime_(uptime)
{}

int Process::Pid() const { return pid_; }

float Process::CpuUtilization() const { return cpu_utilization_; }

string Process::Command() const{ return command_; }

string Process::Ram() const{ return ram_; }

string Process::User() const { return user_; }

long int Process::UpTime() const { return pid_; }

bool Process::operator<(Process const& a) const
{
    return cpu_utilization_ < a.cpu_utilization_;
}
