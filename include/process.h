#ifndef PROCESS_H
#define PROCESS_H

#include <string>
/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
 public:
  Process(int pid,
          std::string const& user,
          std::string const& command,
          float cpu_utilization,
          std::string const& ram,
          long int uptime);

  int Pid() const;
  std::string User() const;
  std::string Command() const;
  float CpuUtilization() const;
  std::string Ram() const;
  long int UpTime() const;
  bool operator<(Process const& a) const;

 private:
    int pid_;
    std::string user_;
    std::string command_;
    float cpu_utilization_;
    std::string ram_;
    long int uptime_;
};

#endif
