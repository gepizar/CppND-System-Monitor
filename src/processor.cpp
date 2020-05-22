#include "processor.h"
#include "linux_parser.h"

#include <iostream>

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() { 

    long total =  LinuxParser::Jiffies();
    long idle = LinuxParser::IdleJiffies(); 
    
    long prevIdle = idle_;
    long prevTotal  = total_; 

    total_ = total; 
    idle_ = idle; 

    return  1.0 - (float)(idle - prevIdle) / (total - prevTotal);  
}
