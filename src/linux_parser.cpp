#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <iomanip>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

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
  string os, version, kernel;
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
float LinuxParser::MemoryUtilization() { 
  // return (MemTotal - MemFree) / MemTotal
  
  string line; 
  string key, value;
  float memtot;
  std::ifstream stream(kProcDirectory + kMeminfoFilename); 

  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line); 
      while (linestream >> key >> value) {
        if (key == "MemTotal:") {
          memtot = stof(value); 
          break; 
        }
        else if (key == "MemFree:") {
          return 1.0 - stof(value) / memtot; 
        }
      }
    }
  }
  return 0.0; 
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() { 
  string line; 
  string value;   

  std::ifstream stream(kProcDirectory + kUptimeFilename); 
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line); 
    linestream >> value; 
  }
  return std::stol(value);
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { 
  return LinuxParser::ActiveJiffies() + LinuxParser::IdleJiffies(); 
}

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
float LinuxParser::CpuUtilization(int pid) { 
  string line; 
  string value;
  float jiffies {0}; 
  float seconds, uptime, starttime, cpu; 
  float hertz = sysconf(_SC_CLK_TCK);

  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename); 
  if (filestream.is_open()) {
    std::getline(filestream, line); 
    std::istringstream linestream(line); 
    for (int idx = 0; idx < 22; idx++) {
      linestream >> value; 
      if (idx == 0) {
        uptime = std::stof(value); 
      }
      else if (idx >= 13 && idx <= 16) {
        jiffies += std::stof(value); 
      }
      else if (idx == 21) {
        starttime = std::stof(value); 
      }
    }
  }
  
  seconds = uptime - (starttime / hertz); 
  cpu = (jiffies / hertz) / seconds; 

  return cpu; 
}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { 
  vector<string> jiffies = LinuxParser::CpuUtilization(); 
  long active {0};
  active += std::stol(jiffies[0]); 
  active += std::stol(jiffies[1]);
  active += std::stol(jiffies[2]);
  active += std::stol(jiffies[5]);
  active += std::stol(jiffies[6]);
  active += std::stol(jiffies[7]); 
  return active; 
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { 
  vector<string> jiffies = LinuxParser::CpuUtilization(); 
  long idle = std::stol(jiffies[3]) + std::stol(jiffies[4]); 
  return idle; 
}


// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() { 
  string line; 
  string value;
  vector<string> jiffies_out; 

  std::ifstream filestream(kProcDirectory + kStatFilename); 
  if (filestream.is_open()) {
    std::getline(filestream, line); 
    std::istringstream linestream(line); 
    linestream >> value; 
    while (linestream >> value) {
      jiffies_out.push_back(value); 
    }
  }
  return jiffies_out;
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() { 
  string line; 
  string key, value; 

  std::ifstream stream(kProcDirectory + kStatFilename); 
  if (stream.is_open()) {
    while(std::getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value){
        if (key == "processes"){
          return std::stoi(value); 
        }
      }
    }
  }
  return 0; 
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() { 
  string line; 
  string key, value; 

  std::ifstream stream(kProcDirectory + kStatFilename); 
  if (stream.is_open()) {
    while(std::getline(stream, line)) {
      std::istringstream linestream(line);

      while (linestream >> key >> value){
        if (key == "procs_running"){
          return std::stoi(value); 
        }
      }
    }
  }
  return 0; 
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) { 
  string line, commandLine; 
  std::ifstream stream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> commandLine; 
  }
  return commandLine; 
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) { 
  string line, key, value;
  float memmb, memkb; 

  std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line); 
      while (linestream >> key >> memkb) {
        if (key == "VmSize:") {
          break; 
        }
      }
    }
  }

  memmb = memkb / 1000;
  std::stringstream mem;
  mem << std::fixed << std::setprecision(1) << memmb; 
  return mem.str(); 
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) { 
  string line, key, value; 
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line); 
      while (linestream >> key >> value) {
        if (key == "Uid:") {
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
  string pid_uid = Uid(pid);
  string line;
  string user, pass, uid;
  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> user >> pass >> uid) {
        if (pid_uid == uid) {
          return user;
        }
      }
    }
  }
  return user; 
}
// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) { 

  long sys_uptime = UpTime();
  string value; 
  string line; 
  int idx; 
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename); 
  if (filestream.is_open()) {
    std::getline(filestream, line); 
    std::istringstream linestream(line);
    for (idx = 0; idx < 22; idx++) {
      linestream >> value; 
    }

    return sys_uptime - std::stol(value) / sysconf(_SC_CLK_TCK); 

  }
  return 0; 
}