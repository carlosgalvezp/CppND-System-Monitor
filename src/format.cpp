#include <string>
#include <sstream>
#include <iomanip>

#include "format.h"

using std::string;

// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long seconds) {
  long hh = seconds / 3600;
  long mm = (seconds - hh * 3600) / 60;
  long ss = seconds - hh * 3600 - mm * 60;

  std::stringstream sstream;

  sstream << std::setw(2) << std::setfill('0') << hh << ":"
          << std::setw(2) << std::setfill('0') << mm << ":"
          << std::setw(2) << std::setfill('0') << ss;

  return sstream.str();
}
