#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>
#include <filesystem>
#include <iostream>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

namespace
{
// Paths
const std::string kProcDirectory{"/proc/"};
const std::string kCmdlineFilename{"/cmdline"};
const std::string kCpuinfoFilename{"/cpuinfo"};
const std::string kStatusFilename{"/status"};
const std::string kStatFilename{"/stat"};
const std::string kUptimeFilename{"/uptime"};
const std::string kMeminfoFilename{"/meminfo"};
const std::string kVersionFilename{"/version"};
const std::string kOSPath{"/etc/os-release"};
const std::string kPasswordPath{"/etc/passwd"};
}  // namespace

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
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
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  for(auto& p: std::filesystem::directory_iterator(kProcDirectory))
  {
    auto dir_name = p.path().filename().string();
    try
    {
      int pid = std::stoi(dir_name);
      pids.push_back(pid);

    }catch (std::invalid_argument&)
    {}
  }
  return pids;
}

float LinuxParser::MemoryUtilization() {
  string line;
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);

  bool found_total_memory{};
  bool found_free_memory{};

  float total_memory{};
  float free_memory{};

  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      if (line.find("MemTotal") != std::string::npos)
      {
        std::istringstream istream(line);
        std::string text;
        std::string memtotal;
        istream >> text >> memtotal;

        total_memory = std::stof(memtotal);
        found_total_memory = true;
      }
      else if (line.find("MemFree") != std::string::npos)
      {
        std::istringstream istream(line);
        std::string text;
        std::string memfree;
        istream >> text >> memfree;

        free_memory = std::stof(memfree);
        found_free_memory = true;
      }

      if (found_total_memory && found_free_memory)
      {
        return free_memory / total_memory;
      }
    }
  }
  return 0.F;
}

long LinuxParser::UpTime() {
  string uptime;
  string line;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> uptime;
  }
  return std::stol(uptime);
}

float LinuxParser::CpuUtilization() {
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    std::string line;

    std::string cpu;
    int user;
    int nice;
    int system;
    int idle;
    int iowait;
    int irq;
    int softirq;
    int steal;
    int guest;
    int guest_nice;

    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> cpu
               >> user
               >> nice
               >> system
               >> idle
               >> iowait
               >> irq
               >> softirq
               >> steal
               >> guest
               >> guest_nice;

    // https://stackoverflow.com/questions/23367857/accurate-calculation-of-cpu-usage-given-in-percentage-in-linux
    int sum_idle = idle + iowait;
    int sum_non_idle = user + nice + system + irq + softirq + steal;
    int total = sum_idle + sum_non_idle;

    return static_cast<float>(total - sum_idle) / static_cast<float>(total);
  }

  return 0.F;
}

int LinuxParser::TotalProcesses() {
  string line;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  std::string const kProcesses = "processes";

  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      auto pos = line.find(kProcesses);
      if (pos != std::string::npos)
      {
        return std::stoi(line.substr(kProcesses.size() + 1));
      }
    }
  }
  return 0;
}

int LinuxParser::RunningProcesses() {
  string line;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  std::string const kProcesses = "procs_running";

  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      auto pos = line.find(kProcesses);
      if (pos != std::string::npos)
      {
        return std::stoi(line.substr(kProcesses.size() + 1));
      }
    }
  }

  return 0;
}

float LinuxParser::CpuUtilization(int pid) {
  int uptime = UpTime();

  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (filestream.is_open()) {
    std::string line;
    std::getline(filestream, line);

    std::istringstream line_stream(line);
    std::string token;
    int token_idx{0};

    int utime{};
    int stime{};
    int cutime{};
    int cstime{};
    int starttime{};

    while (getline(line_stream, token, ' '))
    {
      ++token_idx;

      if (token_idx == 14)
      {
        utime = std::stoi(token);
      }
      else if (token_idx == 15)
      {
        stime = std::stoi(token);
      }
      else if (token_idx == 16)
      {
        cutime = std::stoi(token);
      }
      else if (token_idx == 17)
      {
        cstime = std::stoi(token);
      }
      else if (token_idx == 22)
      {
        starttime = std::stoi(token);
        break;
      }
    }

    // https://stackoverflow.com/questions/16726779/how-do-i-get-the-total-cpu-usage-of-an-application-from-proc-pid-stat/16736599#16736599
    int hertz = sysconf(_SC_CLK_TCK);

    int total_time = utime + stime;
    total_time = total_time + cutime + cstime;

    float seconds = static_cast<float>(uptime) - static_cast<float>(starttime) / static_cast<float>(hertz);
    return (static_cast<float>(total_time) / static_cast<float>(hertz)) / seconds;
  }

  return 0.F;
}

string LinuxParser::Command(int pid) {
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
  if (filestream.is_open()) {
    std::string line;
    std::getline(filestream, line);
    return line;
  }
  return string();
}

string LinuxParser::Ram(int pid) {
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    std::string line;

    while(std::getline(filestream, line))
    {
      if (line.find("VmSize:") != std::string::npos)
      {
        std::istringstream line_stream(line);
        std::string vmsize_title;
        std::string memory;
        line_stream >> vmsize_title >> memory;
        return std::to_string(std::stol(memory) / 1000);
      }
    }
  }
  return string();
}

string LinuxParser::Uid(int pid) {
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    std::string line;

    while(std::getline(filestream, line))
    {
      if (line.find("Uid:") != std::string::npos)
      {
        std::istringstream line_stream(line);
        std::string uid_title;
        std::string uid;
        line_stream >> uid_title >> uid;
        return uid;
      }
    }
  }
  return string();
}

string LinuxParser::User(int pid) {
  std::string uid = Uid(pid);

  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    std::string line;

    while(std::getline(filestream, line))
    {
      if (line.find(":x:" + uid) != std::string::npos)
      {
        return line.substr(0, line.find(":x:"));
      }
    }
  }
  return string();
}

long LinuxParser::UpTime(int pid) {
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (filestream.is_open()) {
    std::string line;
    std::getline(filestream, line);

    std::istringstream line_stream(line);
    std::string token;
    int token_idx{0};

    int starttime{};

    while (getline(line_stream, token, ' '))
    {
      ++token_idx;

      if (token_idx == 22)
      {
        starttime = std::stol(token);
        long hertz = sysconf(_SC_CLK_TCK);
        return starttime / hertz;
      }
    }
  }
  return 0;
}
